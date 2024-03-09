// Virtual Memory Manager // ~ eylon

#include "vmm.h"
#include "pmm.h"
#include "paging.h"
#include <kernel/memory/mm.h>
#include <kernel/process/pm.h>
#include <libc/string.h>
#include <libc/stdint.h>

// [!!!] Control register CR3 holds the page-aligned physical address of a single 4 KB long page directory.

// Pointer to the start of the page table directory; 1024 PDs, one for each process, each holds 1024 PDEs
pde_t* proc_pd = (pde_t*) MM_PTD_START;

// Start and end virtual addresses of the kernel set by the linker
extern char _vstart;
extern char _vend;


// Flush the entire TLB
static inline void tlb_flush() {
    __asm__ volatile ("mov %%cr3, %%eax\n mov %%eax, %%cr3");
}

// Flush a single page from the TLB
static inline void invlpg(vaddr_t virt_base) {
    __asm__ volatile("invlpg (%0)" :: "r" (virt_base) : "memory");
}

// Get the PD of the current running process
pde_t* vmm_get_pd() {
    return &proc_pd[pm_get_pid()];
}

/* !!!!!!!
Note: setting the global (G) bit in a page directory/table entry will 
prevent that entry from being flushed. 
This is useful for pinning interrupt handlers in place.*/

// [TODO] support page size extension (4mb) mapping
// Map a virtual page into a physical address
int vmm_map_page(pde_t* pd, paddr_t phys_base, vaddr_t virt_base, 
    uint8_t rw, uint8_t us, uint8_t pcd)
{
    pde_t* pde = pd + MM_PDE_INDEX(virt_base);     // PDE pointer
    pte_t* pte;                                    // PTE pointer

    // if the PDE is not present
    if (!pde->present) {        
        // allocate a page of physical memory to store the PT and set the PDE to point at it
        pde->pt_address = pmm_get_page() >> 12;
        // [TODO] handle pmm_get_page error

        // zero out the allocated memory
        memset((void*) (pde->pt_address << 12), 0, MM_PAGE_SIZE);
        
        // set the PDE as present
        pde->present = 1;
    }

    // set the PTE pointer by offsetting the PT base address
    pte = (pte_t*) MM_GET_PT(pde) + MM_PTE_INDEX(virt_base);

    // if this PTE is present, it means it's already in use
    if (pte->present) {
        // we must flush the old PDE from the TLB as the data it currently holds won't be true after the remapping
        invlpg(MM_GET_PF(pte));
    }

    // create and set a PTE
    *pte = pte_create(rw, us, pcd, 0, phys_base);

    return 0;
}

// Unmap a virtual page from a given page table directory
int vmm_unmap_page(pde_t* pd, vaddr_t virt_base)
{
    pde_t* pde = pd + MM_PDE_INDEX(virt_base);                       // PDE pointer
    pte_t* pte = (pte_t*) MM_GET_PT(pde) + MM_PTE_INDEX(virt_base);  // PTE pointer

    // set the PTE as not present
    pte->present = 0;
    // flush it from the TLB
    invlpg(MM_GET_PF(pte));

    // [OPT] optimize this by using the 4 available bits in the PDE
    // check for present PTEs in the PT; if there is at least one that is present, exit the function
    for (int i = 0; i < MM_PT_ENTRIES; i++) {
        if (MM_GET_PT(pde)[i].present) return 0;
    }

    /* this part is executed if there are no present PTEs in the PT.
       in this case, free the memory area used by the PT and set the PDE to non present */
    pmm_free_page((paddr_t) MM_GET_PT(pde));
    pde->present = 0;

    return 0;
}

/* Temporarily attach a physical page to the virtual address space of the current
    process and return the virtual address */
vaddr_t vmm_attach_page(paddr_t phys_base)
{
    /* This function (as well as the detach function) assumes that all the reserved pages are 
        in the same 4MB page table area, as they reside above the kernel stack and below the 4MB PTD */
    pde_t* pd  = vmm_get_pd();
    pde_t* pde = pd + MM_PDE_INDEX(MM_RESERVED_START);
    pte_t* pt  = (pte_t*) MM_GET_PT(pde);
    pte_t* pte;

    // if the PDE is not present, use the vmm_map_page function to map the first reserved page
    if (!pde->present) {
        vmm_map_page(pd, phys_base, MM_RESERVED_START, 1, 1, 0);
        return MM_RESERVED_START;
    }

    // search for a free reserved page
    for (uint16_t i = 0; i < MM_RESERVED_PAGES; i++) {
        pte = (pte_t*) pt + (MM_PT_ENTRIES - MM_RESERVED_PAGES - 1 + i);

        // if page is not used (not present), attach the physical page to it
        if (!pte->present) {
            vaddr_t vaddr = MM_RESERVED_START + MM_PAGE_SIZE * i;
            vmm_map_page(pd, phys_base, vaddr, 1, 1, 0);

            return vaddr;
        }
    }    

    // no free page to use, operation failed
    return 0;
}

// Detach temporaraly attached page of the current runnig process
void vmm_detach_page(vaddr_t virt_base)
{
    pde_t* pd  = vmm_get_pd();
    pde_t* pde = pd + MM_PDE_INDEX(MM_RESERVED_START);
    pte_t* pt  = (pte_t*) MM_GET_PT(pde);
    pte_t* pte = pt + MM_PTE_INDEX(virt_base);

    pte->present = 0;
    invlpg(virt_base);
}

void init_vmm()
{
    
}