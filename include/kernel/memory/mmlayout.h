// Memory Layout Header File // ~ eylon

/* This file specifies the memory layout of the operating system. It can be included by 
    both C and Assembly sources.
*/

#if !defined(MMLAYOUT_H)
#define MMLAYOUT_H

/* Utils */

#define KB(x) ((x) * 0x400)
#define MB(x) ((x) * 0x100000)
#define GB(x) ((x) * 0x40000000)

// 4KB page alignment down
#define MM_ALIGN_DOWN(addr) ((addr) - ((addr) % MM_PAGE_SIZE))
// 4KB page alignment up
#define MM_ALIGN_UP(addr)   ((addr) + ((MM_PAGE_SIZE - ((addr) % MM_PAGE_SIZE)) % MM_PAGE_SIZE))

// 4MB page alignment down
#define MM_ALIGN_4MB_DOWN(addr) ((addr) - ((addr) % MM_EXT_PAGE_SIZE))
// 4MB page alignment up
#define MM_ALIGN_4MB_UP(addr)   ((addr) + ((MM_EXT_PAGE_SIZE - ((addr) % MM_EXT_PAGE_SIZE)) % MM_EXT_PAGE_SIZE))

// [align] bytes alignment down
#define MM_ALIGN_X_DOWN (value, align) ((value) - ((value) % (align)))
// [align] bytes alignment up
#define MM_ALIGN_X_UP   (value, align) ((value) + (((align) - ((value) % (align))) % (align)))

/* - - - - - - - - - - - - - - - - - - */
/* - Virtual Address Space Constants - */
/* - - - - - - - - - - - - - - - - - - */

// Page size
#define MM_PAGE_SIZE        ( KB(4) )

// Extended page size
#define MM_EXT_PAGE_SIZE    ( MB(4) )

// PDE count in a page directory
#define MM_PD_ENTRIES       1024
// PTE count in a page table
#define MM_PT_ENTRIES       1024

// Page table directory size; size of all page tables in a page directory
#define MM_PTD_SIZE         ( (MM_PAGE_SIZE) * (MM_PD_ENTRIES) )

// Reserved pages count between the top of the kernel stack and the start of the PTD
#define MM_RESERVED_PAGES   8

// Kernel max stack size (for each process)
#define MM_KSTACK_SIZE      ( KB(8) )

// Kernel stack max pages
#define MM_KSTACK_PAGES     ( (MM_KSTACK_SIZE) / (MM_PAGE_SIZE) )

// Kernel heap max size
#define MM_KHEAP_SIZE       ( MB(1) )

// Kernel heap max pages
#define MM_KHEAP_PAGES      ( (MM_KHEAP_SIZE) / (MM_PAGE_SIZE) )


/* Memory mapped IO area size
    ~ VGA Text Mode Screen: [columns * rows * char_size] = 80 * 25 * 2 = 4000 ≈ (4096 = 4KB) */
#define MM_MMIO_SIZE    ( KB(4) )

// Memory mapped IO pages
#define MM_MMIO_PAGES   ( (MM_MMIO_SIZE) / (MM_PAGE_SIZE) )


// User stack max size
#define MM_USTACK_SIZE  ( MB(8) )

// User heap max size
#define MM_UHEAP_SIZE   ( GB(1) )

// User stack max pages
#define MM_USTACK_PAGES ( (MM_USTACK_SIZE) / (MM_PAGE_SIZE) )

// User heap max pages
#define MM_UHEAP_PAGES  ( (MM_UHEAP_SIZE) / (MM_PAGE_SIZE) )

/* - - - - - - - - - - - - - - - - - - */
/* - Virtual Address Space Addresses - */
/* - - - - - - - - - - - - - - - - - - */

// Top of virtual memory (4GB)
#define MM_VIRT_TOP         0xFFFFFFFF

// Start of the page table directory (list of all page tables in the current page directory)
#define MM_PTD_START        ( (MM_VIRT_TOP) - (MM_PTD_SIZE) + 1 )

// Start of reserved pages area
#define MM_RESERVED_START   ( (MM_PTD_START) - ((MM_PAGE_SIZE) * (MM_RESERVED_PAGES)) )

// Kernel top of stack
#define MM_KSTACK_TOP       ( (MM_RESERVED_START) - 1 )

// Kernel start of heap
#define MM_KHEAP_START      ( (MM_KSTACK_TOP) - (MM_KSTACK_SIZE) - (MM_KHEAP_SIZE) + 1 )

// Memory mapped IO start
#define MM_MMIO_START       ( (MM_KHEAP_START) - (MM_MMIO_SIZE) )

// Higher half start (3GB) / Kernel code, bss, and data start
#define MM_KSPACE_START     0xC0000000

// User top of stack
#define MM_USTACK_TOP       ( (MM_KSPACE_START) - 1 )

// User start of heap
#define MM_UHEAP_START      ( (MM_USTACK_TOP) - (MM_USTACK_SIZE) - (MM_UHEAP_SIZE) + 1 )

// User space start (1MB) / User code, bss and data start
#define MM_USPACE_START     0x100000

/* - - -  - - - - - - - - - - - - - - - */
/* - Physical Address Space Addresses - */
/* - - - - - - - - - - - -  - - - - - - */

// Physical address of the top of the kernel's entry stack
#define MM_PHY_ENTRY_TOS  ( (MM_KSTACK_TOP % MM_EXT_PAGE_SIZE) + MM_EXT_PAGE_SIZE )

#endif