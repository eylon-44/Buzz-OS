// Virtual Memory Manager // ~ eylon

#include "vmm.h"
#include "pmm.h"
#include "paging.h"
#include <kernel/memory/mm.h>
#include <kernel/process/pm.h>
#include <libc/string.h>
#include <libc/stdint.h>

// Pointer to the start of the page table directory; 1024 PDs, one for each process, each holds 1024 PDEs
pde_t (*proc_ptd)[MM_PD_ENTRIES] = (pde_t (*)[MM_PD_ENTRIES]) ((size_t) MM_PTD_START);

// Start and end addresses of the kernel set by the linker
extern char _vstart;
extern char _vend;
extern char _pstart;
extern char _pend;

static bool VMM_INIT_PHASE = true;

// Flush the entire TLB
static inline void tlb_flush() {
    __asm__ volatile ("mov %%cr3, %%eax\n mov %%eax, %%cr3");
}

// Flush a single page from the TLB
static inline void invlpg(vaddr_t virt_base) {
    __asm__ volatile("invlpg (%0)" :: "r" (virt_base) : "memory");
}

static inline void load_pd(paddr_t pd_base) {
    asm volatile("mov %0, %%cr3" : : "r"(pd_base));
}

// Get the PD of the current running process
pde_t* vmm_get_pd() {
    return proc_ptd[pm_get_pid()];
}


// Map a virtual page into a physical address
int vmm_map_page(pde_t* pd, paddr_t phys_base, vaddr_t virt_base, 
    uint8_t rw, uint8_t us, uint8_t pcd, uint8_t glb)
{
    pde_t* pde = pd + MM_PDE_INDEX(virt_base);     // PDE pointer
    pte_t* pte;                                    // PTE pointer
    pte_t* tmp_page = NULL;

    // if the PDE is not present
    if (!pde->present) {        
        // allocate a page of physical memory to store the PT and set the PDE
        *pde = pde_create(pmm_get_page(), rw, us, pcd, 0);

        // temporarly attach the page table so we can access it
        if (!VMM_INIT_PHASE) {
            tmp_page = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde));
        }
        else {
            tmp_page = MM_GET_PT(pde);
        }

        // zero out the allocated memory
        memset((void*) tmp_page, 0, MM_PAGE_SIZE);
    }

    // attach the page table if we didn't already
    if (tmp_page == NULL && !VMM_INIT_PHASE) {
        tmp_page = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde));
    }
    else if (tmp_page == NULL) {
        tmp_page = MM_GET_PT(pde);
    }

    // set the PTE pointer by offsetting the PT base address
    pte = (pte_t*) tmp_page + MM_PTE_INDEX(virt_base);

    // if this PTE is present, it means it's already in use
    if (pte->present) {
        // we must flush the old PDE from the TLB as the data it currently holds won't be true after the remapping
        invlpg(MM_GET_PF(pte));
    }

    // create and set a PTE
    *pte = pte_create(phys_base, rw, us, pcd, glb);

    if (!VMM_INIT_PHASE) {
        // free the temporary page
        vmm_detach_page((vaddr_t) tmp_page);
    }

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

    // check if the PDE should be preserved even if there are no pages in it
    if (pde->_avl & MM_FPDE_PRESERVE) return 0;

    // [OPT] optimize this by using the 2 available bits in the PDE
    // check for present PTEs in the PT; if there is at least one that is present, exit the function
    for (int i = 0; i < MM_PT_ENTRIES; i++) {
        if (MM_GET_PT(pde)[i].present) return 0;
    }

    /* this part is executed if there are no present PTEs in the PT.
       in this case, free the memory area used by the PT and set the PDE to non present. */
    pmm_free_page((paddr_t) MM_GET_PT(pde));
    pde->present = 0;

    return 0;
}

/* Temporarily attach a physical page to the virtual address space of the current
    process and return the virtual address */
vaddr_t vmm_attach_page(paddr_t phys_base)
{
    /* this function (as well as the detach function) assumes that all the reserved pages are 
        in the same 4MB page table area, as they reside above the kernel stack and below the 4MB PTD. 
        these two functions also assume a page table is already set in the reserved pages area. */
    pde_t* pd  = vmm_get_pd();
    pde_t* pde = pd + MM_PDE_INDEX(MM_RESERVED_START);
    pte_t* pt  = (pte_t*) MM_GET_PT(pde);
    pte_t* pte;

    // search for a free reserved page
    for (uint16_t i = 0; i < MM_RESERVED_PAGES; i++) {
        pte = (pte_t*) pt + (MM_PT_ENTRIES - MM_RESERVED_PAGES - 1 + i);

        // if page is not used (not present), attach the physical page to it
        if (!pte->present) {
            *pte = pte_create(phys_base, 1, 0, 0, 0);

            return MM_RESERVED_START + MM_PAGE_SIZE * i;
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


// Initiate the virtual memory manager; map the kernel into each process's PD
void init_vmm()
{
    VMM_INIT_PHASE = true;

    vaddr_t kstart_v = MM_ALIGN_DOWN((size_t) &_vstart);
    vaddr_t kend_v   = MM_ALIGN_UP((size_t) &_vend);
    paddr_t kstart_p = MM_ALIGN_DOWN(((size_t) &_pstart));
    paddr_t kend_p   = MM_ALIGN_UP((size_t) &_pend);

    // allocate a page to hold the first page directory
    pde_t* pd = (pde_t*) pmm_get_page();
    // initiate it with zeros
    memset((void*) pd, 0, MM_PAGE_SIZE);


    /* create page tables for the rest of the kernel space in [pd].
        create page tables to cover the area from [MM_MMIO_START] to [MM_VIRT_TOP].
        note the use of minus one in the for loop's condition; we must do it that way
        because after adding [MM_EXT_PAGE_SIZE] (4MB) to [v] when it is equal to 0xFFC00000
        it results in 0x100000000, which resets down back to 0 because of the 32 bit address 
        limit. this can cause an infinite loop. */
    for (size_t v = MM_ALIGN_4MB_DOWN(MM_MMIO_START); v-1 <= MM_ALIGN_4MB_DOWN(MM_VIRT_TOP); v += MM_EXT_PAGE_SIZE)
    {
        // get the PDE of address [v]
        pde_t* pde = pd + MM_PDE_INDEX(v);

        // if there is already a page table in there, we can skip it
        if (pde->present) continue;

        // create a PDE; allocate a PT and initiate it with zeros
        *pde = pde_create(pmm_get_page(), 1, 0, 0, MM_FPDE_PRESERVE);
        memset(MM_GET_PT(pde), 0, MM_PAGE_SIZE);
    }

    /* map the kernel's code and data into [pd] using the linker labels.
        map virtual [kstart_v]-[kend_v] to physical [kstart_p]-[kend_p]. */
    for (size_t p = kstart_p, v = kstart_v;
            (p < kend_p) && (v < kend_v);
            p += MM_PAGE_SIZE, v += MM_PAGE_SIZE)
    {
        // there is no need to bother setting the available region with [MM_FPDE_PRESERVE] as these pages will never be freed
        vmm_map_page(pd, p, v, 1, 0, 0, 1);
    }


    // map [pd] into the PTD as the first PD
    vmm_map_page(pd, (paddr_t) pd, (vaddr_t) proc_ptd, 1, 0, 0, 1);

    // [TMP][DEBUG][IMPROVE] map the stack
    vmm_map_page(pd, MM_ALIGN_DOWN(0x00090000-1), MM_ALIGN_DOWN(0x00090000-1), 1, 0, 0, 1);

    // switch to [pd]
    load_pd((paddr_t) pd);

    /* NOTE: we can't use [pd] anymore as it points to the physical address of [pd]
        the only way to access [pd] again is by using the [proc_ptd] array */

    VMM_INIT_PHASE = false;

    // allocate the rest of the page directories and map them to the PTD
    for (size_t i = 1; i < PM_MAX_PROCESSES; i++)
    {
        vmm_map_page(proc_ptd[0], pmm_get_page(), (vaddr_t) (proc_ptd[i]), 1, 0, 0, 1);
    }

    // copy [pd] to all the other page directories
    for (size_t i = 1; i < PM_MAX_PROCESSES; i++)
    {
        memcpy((void*) (proc_ptd+i), (void*) proc_ptd[0], MM_PAGE_SIZE);
    }
}