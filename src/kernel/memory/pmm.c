// Physical Memory Manager // ~ eylon

#include <kernel/memory/pmm.h>
#include <kernel/memory/paging.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/mmlayout.h>
#include <kernel/panic.h>
#include <drivers/screen.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <libc/bitfield.h>

// Get the page number from its index in the array
#define INDX_TO_PAGE_NUM(element, index) ((uint32_t) (element)*32+(index))
// Get the page number from its physical base_addr address
#define BASE_TO_PAGE_NUM(base_address)   ((uint32_t) (base_address)/MM_PAGE_SIZE)
/* base_addr -> page number is the index* [TODO] remove me */

/*  Bitmap to keep track of physical pages' state - free(0)/used(1)
    Each bit represents a single page, meaning that bit #i in dword (32bits) #n is page #n*32+i
    As we use an array of 4 byte values and have 1024^2 page frames we need [1024^2/4/8]=0x8000 items */
static uint32_t bitmap[MM_PT_ENTRIES*MM_PT_ENTRIES/sizeof(uint32_t)/8];
/*  [search_start] marks the start of the next search in the physical memory bitmap (array).
    It is initiated with 0. After allocating a page it is set to the value of the next page
    and after deallocating (freeing) a page it is set to the value of the freed page if it is
    less than the current value of [search_start] */
static uint32_t search_start = 0;


// Start and end addresses of the kernel set by the linker
extern char _pstart;
extern char _pend;
extern char _vend;


// Allocate a physical page and return its base_addr address in physical memory
paddr_t pmm_get_page()
{
    // search for the first free physcial page starting from [search_start]
    for (uint32_t n = search_start;; n++) {
        if (n >= sizeof(bitmap)/sizeof(bitmap[0])) n = 0;

        // if the element is not equal to 0xFFFFFFFF it means it has free pages in it (bits set to 0)
        if (bitmap[n] != 0xFFFFFFFF) {
            // find the first free page in the element
            for (uint32_t i = 0; i < 32; i++) {
                // check if the page is free
                if ( BITMAP_GET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i)) == MM_PAGE_FREE ) {
                    // update [search_start], mark the page as used and return its base_addr address
                    search_start = n;
                    BITMAP_SET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i), MM_PAGE_USED);
                    return MM_PAGE_SIZE * INDX_TO_PAGE_NUM(n, i);
                }
            }
        }
    }

    return 0;
}

// Free a physical page by its base_addr address
void pmm_free_page(paddr_t page_base)
{
    // mark the page as free
    BITMAP_SET_BIT(bitmap, BASE_TO_PAGE_NUM(page_base), MM_PAGE_FREE);
    
    /* get the page number in the map to be compared with [search_start]
        becuase [search_start] points to a whole value in an array and not to the exact page number
        we devide the return of [BASE_TO_PAGE_NUM] with the element size of the bitmap */
    uint32_t page_bitmap_index = BASE_TO_PAGE_NUM(page_base) / sizeof(bitmap[0]);
    /* if the freed page's base_addr address is below the base_addr address of the page pointed by [search_start],
        set [search_start] to the location of the freed page so it will start to search from there in the next allocation */
    if (page_bitmap_index < search_start) {
        search_start = page_bitmap_index;
    }
}


/* Ensure that the kernel is not too large 
    We can tell that the kernel is too large when its virtual end address
    overflows into the MM_MEMIO_START region
*/
static void check_ksize()
{
    if ((size_t) &_vend >= MM_MMIO_START) KPANIC("PMM: KERNEL IS TOO LARGE! OVERFLOWS MEMIO");
}

/* Mark all pages in range [start] to [end] as [value]
    [start] and [end] are the BASE of the first and last page to be set to [value] respectively,
    and therefore should be page (4kb) aligned. that means that the page pointed by [end] is included.
*/
static void bitmap_set_range(paddr_t start, paddr_t end, int value)
{
    for (size_t i = BASE_TO_PAGE_NUM(start); i <= BASE_TO_PAGE_NUM(end); i++)
    {
        BITMAP_SET_BIT(bitmap, i, value);
    }
}

// Detect available free physical memory and set the bitmap accordingly
static void mm_detect()
{
    // pointer for a region entry; the first item in the list holds meta data about the list and is not a real entry
    reg_entry_t* reg_entry = (reg_entry_t*) DETECT_BASE;
    size_t entry_count = reg_entry->base_high;

    // start by setting the entire bitmap as used by default
    memset(bitmap, 0xFF, sizeof(bitmap)/sizeof(typeof(bitmap[0])));

    // check that the first entry is valid as described at src/boot/mbr/mm_detect.asm; if not, return
    if (reg_entry->base_low != DETECT_MAGIC || reg_entry->base_high == 0
        || reg_entry->size_low != DETECT_ENTRY_SIZE || reg_entry->size_high != 0xFFFFFFFF
        || reg_entry->type != 0xFFFFFFFF) KPANIC("PMM: CAN'T DETECT AVIALABLE MEMORY");

    // point to the next entry in the list; the first entry was not a real entry and only held meta data
    reg_entry++;

    // go over the entire region entry list
    for (size_t i = 0; i < entry_count; i++, reg_entry++)
    {
        size_t base_addr, top_addr, base_indx, top_indx;

        // if we can use this region
        if (reg_entry->type == REG_USABLE)
        {
            // because we are in 32 bit mode we won't need to use [base_high] and [size_high] in the entry
            base_addr = reg_entry->base_low;

            /* round up [base_addr] and round down [top_addr] if they are not page aligned; this is useful in case they 
                are not aligned and some part of the page belongs to a reserved region. */

            base_addr = MM_ALIGN_UP(base_addr);
            top_addr  = MM_ALIGN_DOWN(base_addr + reg_entry->size_low);

            // set [base_addr] unaligned bitmap pages to free
            for (base_indx = BASE_TO_PAGE_NUM(base_addr); 
                base_indx % (sizeof(bitmap[0])*8) != 0; base_indx++)
            {
                BITMAP_SET_BIT(bitmap, base_indx, MM_PAGE_FREE);
            }

            // set [top_addr] unaligned bitmap pages to free
            for (top_indx = BASE_TO_PAGE_NUM(top_addr);
                top_indx % (sizeof(bitmap[0])*8) != 0; top_indx--)
            {
                BITMAP_SET_BIT(bitmap, top_indx, MM_PAGE_FREE);
            }

            // set aligned bitmap pages between [base_addr] and [top_addr] to free
            memset(bitmap + (base_indx/(sizeof(bitmap[0]) * 8)), 0, (top_indx/8) - (base_indx/8));
        }
    }

    /* mark the first page (physical address 0-4kb) as used
        while its not mendatory, some BIOS stuff that I would prefer not to risk are stored in there */
    BITMAP_SET_BIT(bitmap, 0, MM_PAGE_USED);
}

// Mark the phsycial memory area occupied by the kernel as used in the bitmap
static void kernel_detect()
{
    /* the kernel's physical start and end address can be found at [&_pstart] and [&_pend].
        round the start address and the end address down to align with a page; this ensures
        we don't miss the first page or over occupying the last one. */
    
    size_t start = MM_ALIGN_DOWN((size_t) &_pstart);
    size_t end   = MM_ALIGN_DOWN((size_t) &_pend);

    // mark all pages in range [start] to [end] as used
    bitmap_set_range(start, end, MM_PAGE_USED);
}

// Mark the phsycial memory area occupied by the kernel's entry stack in the bitmap
static void stack_detect()
{
    size_t start = MM_ALIGN_DOWN((size_t) MM_PHY_ENTRY_TOS);
    size_t end   = start;

    // mark all pages in range [start] to [end] as used
    bitmap_set_range(start, end, MM_PAGE_USED);
}

// Mark the physical memory area occupied by memory IO
static void mmio_detect()
{
    // round down page aligned
    size_t start = MM_ALIGN_DOWN(VGA_PHYS_MEM);
    size_t end   = MM_ALIGN_DOWN(VGA_PHYS_MEM + VGA_MEM_SIZE);

    // make all pages in range [start] to [end] as used
    bitmap_set_range(start, end, MM_PAGE_USED);
}

// Initiate the physical memory manager; set the bitmap with used and unused pages
void init_pmm()
{
    check_ksize();
    mm_detect();
    kernel_detect();
    stack_detect();
    mmio_detect();
}