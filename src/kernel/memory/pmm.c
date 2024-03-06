// Physical Memory Manager // ~ eylon

#include "pmm.h"
#include <kernel/memory/mm.h>
#include "paging.h"
#include <libc/stdint.h>
#include <utils/bitfield.h>

/*  Bitmap to keep track of physical pages' state - free(0)/used(1)
    Each bit represents a single page, meaning that bit #i in dword (32bits) #n is page #n*32+i
    As we use an array of 4 byte values and have 1024^2 page frames we need [1024^2/4/8]=0x8000 items */
static uint32_t bitmap[MM_PT_ENTRIES*MM_PT_ENTRIES/sizeof(uint32_t)/8] = { MM_PAGE_FREE };
/*  [search_start] marks the start of the next search in the physical memory bitmap (array).
    It is initiated with 0. After allocating a page it is set to the value of the next page
    and after deallocating (freeing) a page it is set to the value of the freed page if it is
    less than the current value of [search_start] */
static uint32_t search_start = 0;


// Addresses that point to the start and end of the kernel
// [TODO] make this work
extern uint32_t _start;
extern uint32_t _end;


// Allocate a physical page and return its base address in physical memory
paddr_t pmm_get_page()
{
    // search for the first free physcial page starting from [search_start]
    for (uint32_t n = search_start; n < sizeof(bitmap)/sizeof(bitmap[0]); n++) {
        // if the element is not equal to 0xFFFFFFFF it means it has free pages in it (bits set to 0)
        if (bitmap[n] != 0xFFFFFFFF) {
            // find the first free page in the element
            for (uint32_t i = 0; i < 32; i++) {
                // check if the page is free
                if ( BITMAP_GET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i)) == MM_PAGE_FREE ) {
                    // update [search_start], mark the page as used and return its base address
                    search_start = n;
                    BITMAP_SET_BIT(bitmap, INDX_TO_PAGE_NUM(n, i), MM_PAGE_USED);
                    return MM_PAGE_SIZE * INDX_TO_PAGE_NUM(n, i);
                }
            }
        }
    }

    return 0;
}

// Free a physical page by its base address
void pmm_free_page(paddr_t page_base)
{
    // mark the page as free
    BITMAP_SET_BIT(bitmap, BASE_TO_PAGE_NUM(page_base), MM_PAGE_FREE);
    
    // get the page number in the map to be compared with [search_start]
    // becuase [search_start] points to a whole value in an array and not to the exact page number
    // we devide the return of [BASE_TO_PAGE_NUM] with the element size of the bitmap
    uint32_t page_bitmap_index = BASE_TO_PAGE_NUM(page_base) / sizeof(bitmap[0]); 
    // if the freed page's base address is below the base address of the page pointed by [search_start],
    // set [search_start] to the location of the freed page so it will start to search from there in the next allocation
    if (page_bitmap_index < search_start) {
        search_start = page_bitmap_index;
    }
}

// Initiate physical memory manager
void init_pmm()
{
    /* Setup the bitmap with the avialable physical memory pages */
    // 250-450
}