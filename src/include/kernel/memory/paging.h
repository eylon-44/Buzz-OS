// Paging Handler Header File // ~ eylon

#if !defined(PAGING_H)
#define PAGING_H

#include <utils/type.h>

#define PAGE_TABLE_ENTRIES 1024 // number of items in page related tables (pd/pt)
#define PAGE_SIZE          4096 // 4kb :: 0x1000

// Get page directory/table index by virtual address :: max index is 1023 (10 bits)
#define PD_INDEX(virtual_address)  ((virtual_address) >> 22) 
#define PT_INDEX(virtual_address)  (((virtual_address) >> 12) & 0x03FF)
// Get page frame offset :: max 4095/4kb (12 bits)
#define PF_OFFSET(virtual_address) ((virtual_address) & 0x0FFF)


typedef u32_t pte_t; // page table entry
typedef u32_t pde_t; // page directory entry

// Page Table :: A Page Table Entry array :: handles 4MB of physical memory each -> 1024 page frames * 4kb
typedef struct {
    pte_t entries[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE)));
} pt_t;

// Page Directory :: A Page Directory Entry array ::: handles 4GB of physical memory each -> 1024 page tables * 4MB
typedef struct {
    pde_t entries[PAGE_TABLE_ENTRIES]  __attribute__((aligned(PAGE_SIZE)));
} pd_t;

// Page Table Entry Flags
typedef enum {
    PTE_PRESENT      = 0x01,       // [bit 0] present in RAM
    PTE_WRITE_ACCESS = 0x02,       // [bit 1] read/write bit, 0=read 1=read/write
    PTE_USER_ACCESS  = 0x04,       // [bit 2] user/supervisor, 0=[ring 0] 1=[all rings]
    PTE_PWT          = 0x08,       // [bit 3] page-level write-through
    PTE_PCD          = 0x10,       // [bit 4] page-level cache-disabled
    PTE_ACCESSED     = 0x20,       // [bit 5] accessed
    PTE_DIRTY        = 0x40,       // [bit 6] dirty, whether the page has been written to
    PTE_PAT          = 0x80,       // [bit 7] page attribute table, must be 0 unless PAT supported
    PTE_GLOBAL       = 0x100,      // [bit 8] global translation
    PTE_AVL = 0b111000000000,      // [bit 9-11] available filter: these bits are ignored by the cpu
    PTE_PF_ADDRESS   = 0xFFFFF000, // [bit 12-31] filter: physical address filter of 4KB page frame
} PTE_FLAGS;

// Page Directory Entry Flags
typedef enum {
    PDE_PRESENT      = 0x01,       // [bit 0] present in RAM
    PDE_WRITE_ACCESS = 0x02,       // [bit 1] read/write bit, 0=read 1=read/write
    PDE_USER_ACCESS  = 0x04,       // [bit 2] user/supervisor, 0=[ring 0] 1=[all rings]
    PDE_PWT          = 0x08,       // [bit 3] page-level write-through
    PDE_PCD          = 0x10,       // [bit 4] page-level cache-disabled
    PDE_ACCESSED     = 0x20,       // [bit 5] accessed
    PDE_PS4MB        = 0x80,       // [bit 7] page size, 0=4KB 1=4MB :: when set the page directory almost acts like a PTE that points to a 4MB page frame
    PDE_AVL = 0b111101000000,      // [bit 6, 8-11] available filter: these bits are ignored by the cpu
    PDE_PT_ADDRESS   = 0xFFFFF000, // [bit 12-31] filter: physical address filter of 4KB aligned page table referenced by this entry
} PDE_FLAGS;

void init_paging();

#endif