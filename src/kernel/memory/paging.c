// Paging Hanlder // ~ eylon

#include <kernel/memory/paging.h>
#include <utils/bitfield.h>
#include <utils/type.h>

/* The page directory is initiated with the first 4MB of physcial address mapped to
   itself and 0xC0000000 (3GB/Higher Half Kernel) */
pde_t page_directory[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE))) = {
    [0]                PDE_PS4MB | PTE_PRESENT | PTE_WRITE_ACCESS,
    [0xC0000000 >> 22] PDE_PS4MB | PTE_PRESENT | PTE_WRITE_ACCESS
};


// Addresses that point to the start and end of the kernel
// [TODO] make this work
extern u32_t _start;
extern u32_t _end;

/*  Bitmap to keep track of physical pages' state - free(0)/used(1)
    Each bit represents a single page, meaning that bit #i in dword (32bits) #n is page #n*32+i
    As we use an array of 4 byte values and have 1024^2 page frames we need [1024^2/4/8]=0x8000 items */
static u32_t bitmap[PAGE_TABLE_ENTRIES*PAGE_TABLE_ENTRIES/sizeof(u32_t)/8];
/*  [search_start] marks the start of the next search in the physical memory bitmap (array)
    starts at 0, after allocating a page is set to the value of the next page,
    after deallocating (freeing) a page is set to the value of the freed page */
static u32_t search_start = 0;


/* [TODO][REMOVE][DEBUG] */
static physical_address pmm_get_page() __attribute__((unused));
static void pmm_free_page(physical_address page_base) __attribute__((unused));


// Allocate a physical page and return its base address in physical memory
static physical_address pmm_get_page()
{
    // search for the first free physcial page starting from [search_start]
    for (u32_t n = search_start; n < sizeof(bitmap)/sizeof(bitmap[0]); n++) {
        // if the element is not equal to 0xFFFFFFFF it means it has free pages in it (bits set to 0)
        if (bitmap[n] != 0xFFFFFFFF) {
            // find the first free page in the element
            for (u32_t i = 0; i < 32; i++) {
                // check if the page is free
                if ( BITMAP_GET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i)) == PAGE_FREE ) {
                    // update [search_start], mark the page as used and return its base address
                    search_start = n;
                    BITMAP_SET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i), PAGE_USED);
                    return PAGE_SIZE * INDX_TO_PAGE_NUM(n, i);
                }
            }
        }
    }
}

// Free a physical page by its base address
static void pmm_free_page(physical_address page_base)
{
    // mark the page as free
    BITMAP_SET_BIT(bitmap, BASE_TO_PAGE_NUM(page_base), PAGE_FREE);
    
    // get the page number in the map to be compared with [search_start]
    // becuase [search_start] points to a whole value in an array and not to the exact page number
    // we devide the return of [BASE_TO_PAGE_NUM] with the element size of the bitmap
    u32_t page_bitmap_index = BASE_TO_PAGE_NUM(page_base) / sizeof(bitmap[0]); 
    // if the freed page's base address is below the base address of the page pointed by [search_start],
    // set [search_start] to the location of the freed page so it will start to search from there in the next allocation
    if (page_bitmap_index < search_start) {
        search_start = page_bitmap_index;
    }
}

void init_paging()
{
    // [TODO]
}