// Paging Hanlder // ~ eylon

#include <kernel/memory/mm.h>
#include <kernel/memory/gdt.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <libc/stddef.h>

// Initiate Memory Manager
void init_mm()
{
    init_gdt();
    init_pmm();
    init_vmm();
    init_kheap();
}