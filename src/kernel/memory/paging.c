// Paging Utils // ~ eylon

#include "paging.h"
#include <kernel/memory/mm.h>
#include <libc/stdint.h>
#include <libc/stdbool.h>

// Create a page table entry
pte_t pte_create(bool rw, bool us, bool pcd, bool global, paddr_t pf_address)
{
    pte_t pte = {
        .present    = true,
        .accessed   = false,
        .dirty      = false,
        ._avl0      = 0,
        .rw         = rw,
        .us         = us,
        .pwt        = false,
        .pcd        = pcd,
        .pat        = false,
        .global     = global,
        .pf_address = (pf_address >> 12)
    };

    return pte;
}

// Create a page directory entry for 4kb page frames (ps is false)
pde_t pde_create(bool rw, bool us, bool pcd, paddr_t pt_address)
{
    pde_t pde = {
        .present    = true,
        .accessed   = false,
        ._avl0      = 0,
        ._avl1      = 0,
        .rw         = rw,
        .us         = us,
        .pwt        = false,
        .pcd        = pcd,
        .ps         = false,
        .pt_address = (pt_address >> 12)
    };

    return pde;
}