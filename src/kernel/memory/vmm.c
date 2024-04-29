// Virtual Memory Manager // ~ eylon

#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/paging.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/mmlayout.h>
#include <kernel/process/pm.h>
#include <drivers/screen.h>
#include <libc/string.h>
#include <libc/stdint.h>

/* Pointer to an array of 1024 PDEs, or a single PD.
    This pointer holds the base address of the PD-per-process list */
pde_t* proc_ptd[MM_PD_ENTRIES]; // [HOLUP] do I really need this? can't just each process point to its own pd?

// Start and end addresses of the kernel set by the linker
extern char _vstart;
extern char _vend;
extern char _pstart;
extern char _pend;


// Flush the entire TLB
static inline void tlb_flush() {
    __asm__ volatile ("mov %%cr3, %%eax\n mov %%eax, %%cr3");
}

// Flush a single page from the TLB
static inline void invlpg(vaddr_t virt_base) {
    __asm__ volatile("invlpg (%0)" :: "r" (virt_base) : "memory");
}

// Switch a page directory
static inline void load_pd(paddr_t pd_base) {
    asm volatile("mov %0, %%cr3" : : "r"(pd_base));
}

// Get a physical address pointer of the PD of the current active process
pde_t* vmm_get_pd() {
    return proc_ptd[pm_get_pid()];
}


// Map a virtual page into a physical address
int vmm_map_page(pde_t* pd, paddr_t phys_base, vaddr_t virt_base, 
    uint8_t rw, uint8_t us, uint8_t pcd, uint8_t glb)
{
    pde_t* pde = (pde_t*) vmm_attach_page((paddr_t) pd) + MM_PDE_INDEX(virt_base);
    pte_t* pt = NULL;
    pte_t* pte;

    // if the PDE is not present
    if (!pde->present) {
        // allocate a page of physical memory to store the PT and set the PDE
        *pde = pde_create(pmm_get_page(), rw, us, pcd, 0);

        // temporarly attach the page table so we can access it
        pt = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde));

        // zero out the allocated memory
        memset((void*) pt, 0, MM_PAGE_SIZE);
    }

    // attach the page table if we didn't already
    if (pt == NULL) {
        pt = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde));
    }

    // set the PTE pointer by offsetting the PT base address
    pte = (pte_t*) pt + MM_PTE_INDEX(virt_base);

    // if this PTE is present, it means it's already in use
    if (pte->present) {
        // we must flush the old PDE from the TLB as the data it currently holds won't be true after the remapping
        invlpg(MM_GET_PF(pte));
    }

    // create and set a PTE
    *pte = pte_create(phys_base, rw, us, pcd, glb);
    
    // free the temporary attached pages
    vmm_detach_page((vaddr_t) pde);
    vmm_detach_page((vaddr_t) pt);

    return 0;
}

// Unmap a virtual page from a given page table directory
void vmm_unmap_page(pde_t* pd, vaddr_t virt_base)
{
    pde_t* pde = (pde_t*) vmm_attach_page((paddr_t) pd) + MM_PDE_INDEX(virt_base);
    pte_t* pte = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde)) + MM_PTE_INDEX(virt_base); 

    // if PDE is not present, the page is not mapped so this function can't continue
    if (!pde->present) goto exit;

    // set the PTE as not present
    pte->present = 0;
    // flush it from the TLB
    invlpg(MM_GET_PF(pte));

    // don't continue if the PDE should be preserved even if there are no pages in it
    if (pde->_avl & MM_AVL_PDE_KEEP) goto exit;

    // [OPT] optimize this by using the 2 available bits in the PDE
    // check for present PTEs in the PT; if there is at least one that is present, exit the function
    for (int i = 0; i < MM_PT_ENTRIES; i++) {
        if (MM_GET_PT(pde)[i].present) goto exit;
    }

    /* this part is executed only if there are no present PTEs in the PT.
       in this case, free the memory area used by the PT and set the PDE to non present. */
    pmm_free_page((paddr_t) MM_GET_PT(pde));
    pde->present = 0;

    // free temporary attached pages and return
    exit:
        vmm_detach_page((vaddr_t) pde);
        vmm_detach_page((vaddr_t) pte);
        return;
}

/* Temporarily attach a physical page to the virtual address space of the current
    process and return the virtual address */
vaddr_t vmm_attach_page(paddr_t phys_base)
{
    /* this function (as well as the detach function) assumes that all the reserved pages are 
        in the same 4MB page table area, as they reside above the kernel stack and below the 4MB PTD. 
        these two functions also assume a page table is already set in the reserved pages area. */
    const pte_t* pt = (pte_t*) ((pte_t(*)[MM_PD_ENTRIES]) MM_PTD_START + MM_PDE_INDEX(MM_RESERVED_START));
    pte_t* pte;

    // search for a free reserved page
    for (size_t i = 0; i < MM_RESERVED_PAGES; i++) {
        pte = (pte_t*) pt + (MM_PT_ENTRIES - MM_RESERVED_PAGES + i);

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
    const pte_t* pt = (pte_t*) ((pte_t(*)[MM_PD_ENTRIES]) MM_PTD_START + MM_PDE_INDEX(MM_RESERVED_START));
    pte_t* pte = (pte_t*) pt + MM_PTE_INDEX(virt_base);

    pte->present = 0;
    invlpg(virt_base);
}

// Delete a page directory
void vmm_del_dir(pde_t* pd)
{
    pde_t* pd_p = (pde_t*) vmm_attach_page((paddr_t) pd); 

    // Go over all the page directory entries
    for (int i = 0; i < MM_PD_ENTRIES; i++)
    {
        pte_t* pt;

        // If PDE is present, seek for present PTEs inside it
        if (pd_p[i].present) {
            pt = vmm_attach_page((paddr_t) MM_GET_PT(pd+i));    // map the PDE's page table in order to access it

            // Go over all the page directory entries
            for (int k = 0; k < MM_PT_ENTRIES; k++)
            {
                // If PTE is present, free it
                if (pt[k].present) {
                    pmm_free_page((paddr_t) MM_GET_PF(pt+k));
                }
            }

            vmm_detach_page((vaddr_t) pt);                      // free the temporarly attached page
            pmm_free_page((paddr_t) MM_GET_PT(pd+i));           // free the physical page of the page table
        }
    }

    vmm_detach_page((vaddr_t) pd_p);                            // free the temporarly attached page
    pmm_free_page(pd);                                          // free the page directory
}

// Check if a page is mapped or not in a certain page directory; returns 1 for true and 0 for false
int vmm_is_mapped(pde_t* pd, vaddr_t virt_base)
{
    pde_t* pde = (pde_t*) vmm_attach_page((paddr_t) pd) + MM_PDE_INDEX(virt_base);
    pte_t* pte = (pte_t*) vmm_attach_page((paddr_t) MM_GET_PT(pde)) + MM_PTE_INDEX(virt_base); 
    
    int is_mapped = pde->present && pte->present;

    vmm_detach_page((vaddr_t) pde);
    vmm_detach_page((vaddr_t) pte);
    return is_mapped;
}

// Initiate the virtual memory manager; map the kernel into each process's PD
void init_vmm()
{
    /* This function can't use the vmm_map_page, vmm_unmap_page, vmm_attach_page and vmm_detach_page
        functions before the PD of the first process is being loaded. */

    /* Physical and virtual addresses of the kernel's boundaries */
    vaddr_t kstart_v = MM_ALIGN_DOWN((size_t) &_vstart);
    vaddr_t kend_v   = MM_ALIGN_UP((size_t) &_vend);
    paddr_t kstart_p = MM_ALIGN_DOWN(((size_t) &_pstart));
    paddr_t kend_p   = MM_ALIGN_UP((size_t) &_pend);

    /* FOR NOW we can keep a list of pointers for the PDs but LATER when we have process structures,
        keep the PD address in the structure
    */

    // allocate a page to hold the first page directory
    pde_t* pd = (pde_t*) pmm_get_page();
    // initiate it with zeros
    memset((void*) pd, 0, MM_PAGE_SIZE);


    /* map the kernel's code and data into [pd] using the linker labels.
        map virtual [kstart_v]-[kend_v] to physical [kstart_p]-[kend_p]. */
    for (size_t p = kstart_p, v = kstart_v;
            (p < kend_p) && (v < kend_v);
            p += MM_PAGE_SIZE, v += MM_PAGE_SIZE)
    {
        // find the PDE
        pde_t* pde = (pde_t*) pd + MM_PDE_INDEX(v);
        pte_t* pte;
        
        // if there is no page table in that area already create an empty one
        if (!pde->present) {
            *pde = pde_create(pmm_get_page(), 1, 0, 0, MM_AVL_PDE_KEEP);
            memset(MM_GET_PT(pde), 0, MM_PAGE_SIZE);
        }

        // find and set the PTE
        pte  = (pte_t*) MM_GET_PT(pde) + MM_PTE_INDEX(v);
        *pte = pte_create(p, 1, 0, 0, 1);
    }
    

    /* create empty page tables for the rest of the kernel space in [pd].
        create page tables to cover the area from [MM_MMIO_START] to [MM_VIRT_TOP]. */
    for (size_t v = MM_ALIGN_4MB_DOWN(MM_MMIO_START); v != MM_ALIGN_4MB_UP(MM_VIRT_TOP); v += MM_EXT_PAGE_SIZE)
    {
        // get the PDE of address [v]
        pde_t* pde = pd + MM_PDE_INDEX(v);

        // if there is already a page table in there, skip it
        if (pde->present) continue;

        // create a PDE; allocate a PT and initiate it with zeros
        *pde = pde_create(pmm_get_page(), 1, 0, 0, MM_AVL_PDE_KEEP);
        memset(MM_GET_PT(pde), 0, MM_PAGE_SIZE);
    }

    /* map the last 4MB of memory (from [MM_PTD_START] to [MM_VIRT_TOP]) to all present page 
        tables of current PD. [pi] is the page table index in the PD that should be mapped 
        to virtual address [v]. */
    for (size_t v = MM_PTD_START, pi = 0; v != MM_ALIGN_UP(MM_VIRT_TOP); v += MM_PAGE_SIZE, pi++)
    {
        // find the PDE to map the page table into
        pde_t* pde = (pde_t*) pd + MM_PDE_INDEX(v);
        // find the PDE that points to the page table we want to map into [pde]
        pde_t* pde_to_map = (pde_t*) pd + pi;
        pte_t* pte;

        // if there is no page table in that area continue
        if (!pde_to_map->present) continue;

        // find and set the PTE
        pte  = (pte_t*) MM_GET_PT(pde) + MM_PTE_INDEX(v);
        *pte = pte_create((paddr_t) MM_GET_PT(pde_to_map), 1, 0, 0, 1);
    }

    /* map the entry stack; assuming the current stack's size is not over 4KB */
    {
        pde_t* pde = (pde_t*) pd + MM_PDE_INDEX(MM_KSTACK_TOP);
        pte_t* pte = (pte_t*) MM_GET_PT(pde) + MM_PTE_INDEX(MM_KSTACK_TOP);
        *pte = pte_create(MM_PHY_ENTRY_TOS, 1, 0, 0, 0);
    }


    // switch to [pd]
    load_pd((paddr_t) pd);

    /* NOTE: we can't use [pd] directly anymore as it points to the physical address 
        of [pd]. the only way to access [pd] again is by attaching it to a temporary page
        using the vmm_attach_page function. Also note, we can now use all of the 
        vmm's functions. */

    // map memory mapped IO
    vmm_map_page(pd, VGA_PHYS_MEM, VGA_VIRT_MEM, 1, 0, 0, 1);

    // map the TSS
    vmm_map_page(pd, pmm_get_page(), MM_TSS_START, 1, 0, 0, 1);

    // save [pd]
    proc_ptd[pm_get_pid()] = pd;
}