// Physical Memory Manager Header File // ~ eylon

#if !defined(PMM_H)
#define PMM_H

#include <kernel/memory/mm.h>

// Free & Used page states in the bitmap
#define MM_PAGE_FREE 0x0
#define MM_PAGE_USED 0x1

typedef struct __attribute__((packed))
{
    uint32_t base_low;
    uint32_t base_high;
    uint32_t size_low;
    uint32_t size_high;
    uint32_t type;
} mm_region_t ;


paddr_t pmm_get_page();
void pmm_free_page(paddr_t page_base);
void init_pmm();

#endif