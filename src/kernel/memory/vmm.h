// Virtual Memory Manager Header File // ~ eylon

#if !defined(VMM_H)
#define VMM_H

#include <kernel/memory/mm.h>
#include <libc/stdint.h>
#include "paging.h"

void init_vmm();
pde_t* vmm_get_pd();
int vmm_map_page(pde_t* pd, paddr_t phys_base, vaddr_t virt_base, uint8_t rw, uint8_t us, uint8_t pcd, uint8_t glb);
int vmm_unmap_page(pde_t* pd, vaddr_t virt_base);
vaddr_t vmm_attach_page(paddr_t phys_base);
void vmm_detach_page(vaddr_t virt_base);

#endif