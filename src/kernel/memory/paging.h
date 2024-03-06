// Paging Utils Header File // ~ eylon

#if !defined(PAGING_H)
#define PAGING_H

#include <kernel/memory/mm.h>
#include <libc/stdint.h>
#include <libc/stdbool.h>

/* Constants & Convertions */

// PDE count in a page directory
#define MM_PD_ENTRIES 1024
// PTE count in a page table
#define MM_PT_ENTRIES 1024
// Page size (4kb)
#define MM_PAGE_SIZE  4096

/* Virtual -> Index Conversions */
// Get PDE index from virtual address           :: max index is 1023 (10 bits)
#define MM_PDE_INDEX(virtual_address) (((virtual_address) >> 22) & 0x03FF)
// Get PTE index from virtual address           :: max index is 1023 (10 bits)
#define MM_PTE_INDEX(virtual_address) (((virtual_address) >> 12) & 0x03FF)
// Get offset within a PF from virtual address  :: max offset is 4095/4kb-1 (12 bits)
#define MM_PF_OFFSET(virtual_address) ((virtual_address) & 0x0FFF)


#define MM_GET_PT(pde_ptr)  ((pte_t*)  ((pde_ptr)->pt_address << 12))
#define MM_GET_PF(pte_ptr)  ((paddr_t) ((pte_ptr)->pf_address << 12))

#define MM_GET_PHYSICAL(virtual_address, pd) ( \
    (paddr_t) \
    MM_GET_PF( \
        MM_GET_PT((pd) + MM_PDE_INDEX(virtual_address)) \
        + MM_PTE_INDEX(virtual_address) ) \
    | MM_PF_OFFSET(virtual_address) )


/* Types */

// Page Table Entry (PTE) type
typedef struct __attribute__((packed)) {
    uint8_t present:1;        // [bit 0] present in RAM
    uint8_t rw:1;             // [bit 1] read/write
    uint8_t us:1;             // [bit 2] user/supervisor
    uint8_t pwt:1;            // [bit 3] page-level write-through
    uint8_t pcd:1;            // [bit 4] page-level cache-disabled
    uint8_t accessed:1;       // [bit 5] accessed
    uint8_t dirty:1;          // [bit 6] dirty
    uint8_t pat:1;            // [bit 7] page attribute table, must be 0 unless PAT supported
    uint8_t global:1;         // [bit 8] global translation
    uint8_t _avl0:3;          // [bit 9-11]  available: ignored by cpu
    uint32_t pf_address:20;   // [bit 12-31] physical base address of 4KB page frame
} pte_t;

// Page Directory Entry (PDE) for 4kb page frames type
typedef struct __attribute__((packed)) {
    uint8_t present:1;        // [bit 0] present in RAM
    uint8_t rw:1;             // [bit 1] read/write
    uint8_t us:1;             // [bit 2] user/supervisor
    uint8_t pwt:1;            // [bit 3] page-level write-through
    uint8_t pcd:1;            // [bit 4] page-level cache-disabled
    uint8_t accessed:1;       // [bit 5] accessed
    uint8_t _avl0:1;          // [bit 6] available: ignored by cpu
    uint8_t ps:1;             // [bit 7] page size extension, 0=4KB 1=4MB, must be [0] for this struct
    uint8_t _avl1:4;          // [bit 8-11]  available: ignored by cpu
    uint32_t pt_address:20;   // [bit 12-31] physical address of 4KB aligned page table referenced by this entry
} pde_t;

// Page Directory Entry 4MB (PDE-4MB) type [TODO] ???
typedef struct __attribute__((packed)) {
    uint8_t present:1;        // [bit 0] present in RAM
    uint8_t rw:1;             // [bit 1] read/write
    uint8_t us:1;             // [bit 2] user/supervisor
    uint8_t pwt:1;            // [bit 3] page-level write-through
    uint8_t pcd:1;            // [bit 4] page-level cache-disabled
    uint8_t accessed:1;       // [bit 5] accessed
    uint8_t dirty:1;          // [bit 6] dirty
    uint8_t ps:1;             // [bit 7] page size extension, 0=4KB 1=4MB, must be [1] for this struct
    uint8_t global:1;         // [bit 8] global translation
    uint8_t _avl0:3;          // [bit 9-11]  available: ignored by cpu
    uint8_t pat:1;            // [bit 12] page attribute table, must be 0 unless PAT supported
    uint16_t _pf_address:8;   // [bit 13-20] physical base address of 4MB page frame
    uint8_t _reserved:1;      // [bit 21] reserved [0]
    uint16_t pf_address:10;   // [bit 22-31] physical base address of 4MB page frame
} pde4mb_t;

// Page Table type           :: 1024 page table entries, each points to a page frame
typedef pte_t pt_t[MM_PT_ENTRIES] __attribute__((aligned(MM_PAGE_SIZE)));
// Page Table Direcotyr type :: 1024 page directory entries, each points to a page table
typedef pde_t ptd_t[MM_PT_ENTRIES]  __attribute__((aligned(MM_PAGE_SIZE)));


/* Functions */

pte_t pte_create(bool rw, bool us, bool pcd, bool global, paddr_t pf_address);
pde_t pde_create(bool rw, bool us, bool pcd, paddr_t pt_address);

#endif