// Physical Memory Manager Header File // ~ eylon

#if !defined(PMM_H)
#define PMM_H

#include <kernel/memory/mm.h>

// Free & Used page states in the bitmap
#define MM_PAGE_FREE 0x0
#define MM_PAGE_USED 0x1

// Base addres of detected memory list
#define DETECT_BASE       0x8000
// Detected memory list entry size
#define DETECT_ENTRY_SIZE 20
// BIOS INT 0x14 magic
#define DETECT_MAGIC      0x534D4150

// Top of low memory
#define MM_LOW_MEM_TOP 0xFFFFF
// VGA screen physical video address
#define MM_VGA_BASE 0xB8000
#define MM_VGA_SIZE 0x1000

// Physical memory region types
typedef enum
{
    REG_USABLE      = 1,        // usable free memory
    REG_RESERVED    = 2,        // unusable reserved memory
    REG_RECLAIMABLE = 3,        // reclaimable memory; we can reclaim it if we don't need the BIOS anymore
    REG_ACPI_NVS    = 4,        // unusable
    REG_BAD_MEM     = 5         // unusable bad memory
} reg_type_t;

// Physcial memory region entry struct
typedef struct __attribute__((packed))
{
    uint32_t base_low;      // region base address low
    uint32_t base_high;     // region base address high (we will probably never use that as we are in 32 bit mode)
    uint32_t size_low;      // region size low
    uint32_t size_high;     // region size high (we will probably never use that as we are in 32 bit mode)
    uint32_t type;          // regiong type
} reg_entry_t;


paddr_t pmm_get_page();
void pmm_free_page(paddr_t page_base);
void init_pmm();

#endif