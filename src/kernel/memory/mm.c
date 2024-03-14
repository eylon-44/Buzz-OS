// Paging Hanlder // ~ eylon

#include <kernel/memory/mm.h>
#include "gdt.h"
#include "pmm.h"
#include "vmm.h"
#include "heap.h"

// Initiate Memory Manager
void init_mm()
{
    init_gdt();
    init_pmm();
    init_vmm();
}

/*
GDT
    [X] init:     load the GDT descriptor into the GDTR
PMM
    [X] init:     set the bitmap with available pages in the ram
    [X] detect:   detect available memory (set _start and _end variables) (only possible when loading the kernel with as an ELF)
    [X] get:      get an available physical page
    [X] free:     free a given physical page
    [ ] is present

VMM
    [ ] init:     set the page tables for the first process (pid 0, which is used for the initial identity paging).
                    initiate the kernel's stack and heap.
    [X] identity paging
    [X] map:      map a given virtual address into a given physical address
    [X] unmap:    unmap a given virtual address
    [X] invlpg:   flush that TLB!
    [X] attach:   attach a physical page into a reserved virtual address space in order to access it temporarly
    [X] detach:   detach that physical page from the reserved  virtual address space
    [X] convert:  convert virtual addresses to physical ones and the other way around (v->p, p->v)
HEAP
    [ ] init: allocate an initial page for the heap
    [X] kmalloc:
    [ ] kmalloc aligned:
    [X] kfree:
    [ ] krealloc:
    [X] kextend heap
MM
    [X] init:     init gdt -> pmm -> vmm -> heap
PAGING
    [X] pte_t, pt_t, pde_t, ptd_t types
    [ ] 4MB page tables ???
    [X] create page tables and directories utils

[ ] Who handles page faults?


Virtual memory within each address space:

    ---------------------------------------------------- <------- MM_VIRT_TOP (4GB, 0xFFFF:FFFF)
    |                                                  |
    |          Page Tables of Current Process          | -------> MM_PD_ENTRIES (1024) pages (4MB)
    |                                                  |
    ---------------------------------------------------- <------- MM_PTD_START (0xFFC0:0000)
    |                                                  |
    |               Reserved Free Pages                | -------> MM_RESERVED_PAGES pages :: Used for temporarily attaching pages
    |                                                  |
    ---------------------------------------------------- <------- MM_RESERVED_START / MM_KSTACK_TOP
    |                                                  |
    |                   Kernel Stack                   | -------> MM_KSTACK_PAGES pages
    |                                                  |
    ----------------------------------------------------
    |                                                  |
    |                   Kernel Heap                    | -------> MM_KHEAP_PAGES pages
    |                                                  |
    ---------------------------------------------------- <------- MM_KHEAP_START
    |                                                  |
    |                 Memory Mapped IO                 |
    |                                                  |
    ---------------------------------------------------- <------- MM_MMIO_START
    |                                                  |
    |            Kernel Code, Data and BSS             |
    |                                                  |
    ---------------------------------------------------- <------- MM_KSPACE_START (3GB, 0xC0000000) / MM_USTACK_TOP
    |                                                  |
    |                    User Stack                    | -------> MM_USTACK_PAGES pages
    |                                                  |
    ----------------------------------------------------
    |                                                  |
    |                    User Heap                     | -------> MM_UHEAP_PAGES pages
    |                                                  |
    ---------------------------------------------------- -------> MM_UHEAP_START
    |                                                  |
    |             User Code, Data and BSS              |
    |                                                  |
    ---------------------------------------------------- <------- MM_USPACE_START (1MB)
    |                                                  |
    |                    Low memory                    |
    |                                                  |
    ---------------------------------------------------- <------- 0x0000:0000

*/