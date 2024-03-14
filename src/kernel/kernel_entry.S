// Kernel Entry // ~ eylon

.code32
.global _start
.extern kernel_main
.extern gdt_descriptor
.extern proc_pd


// Kernel's virtual and physical bases are used to convert virtual addresses to physical ones
#define KVIRT_BASE 0xC0100000
#define KPHYS_BASE 0x100000

// Convert virtual address to physical
#define V2P(vaddr)  ((vaddr) - (KVIRT_BASE - KPHYS_BASE))

// Scratch space for placing the entry page direcotry in
#define ENTRY_PD_BASE   0x10000
// PDE count in a single PD
#define PDE_COUNT 1024 // coimment
// PDE size in bytes
#define PDE_SIZE  4
// PDE flags: [bit 0] present, [bit 1] read & write, [bit 7] page size extension
#define PDE_FLAGS 0x83
// Size of a single page frame; 4MB because of the page size extension
#define PF_SIZE 0x400000

// Physical address of [_entry]; the bootloader should jump to this address because paging is still off
.equ _start, V2P(_entry)

// Kernel's entry point
_entry:
    cli                     // disable interrupts
    call enable_paging      // enable paging
    call setup_gdt          // setup the gdt

    // set a temporary stack location until memory initialization takes place
    movl $0x0008fffc, %ebp
    movl %ebp, %esp

    // jump to the kernel's main function in the higher half
    // use an indirect call because the assembler produces a PC-relative instruction for a direct jump
    movl $kernel_main, %eax
	call *%eax

    // This code should never run
    jmp .


// Setup the GDT
setup_gdt:
    // load the gdt
    lgdt (gdt_descriptor)
    ret

// Enable paging and set a basic startup page directory
enable_paging:
    
    // generate entry page directory
    // identity map the entire address space except 3GB using 4MB pages; map 3GB to physical address 0

    // identity map the entire address space
    movl $ENTRY_PD_BASE, %ebx       // page direcotry base address
    movl $PDE_COUNT, %ecx           // number of page directory entries in a single page directory
    movl $0, %edx                   // EDX will keep track of the current physical address; initiate with 0

    .loop_pd:
        movl $PDE_FLAGS, (%ebx)     // set the PDE's flags
        orl %edx, (%ebx)            // set the corresponding physical address in order to achieve identity paging
        addl $PDE_SIZE, %ebx        // increase EBX by the size of a page directory entry in order to point the next one
        addl $PF_SIZE, %edx         // increase EDX by the size of an extended page frame (4MB) so it will point at the next physical base
        loop .loop_pd               // loop

    // map the first 4MB of virtual address 3GB to physical 0 [v: 3GB-(3GB+4MB) = p: 0-4MB]
    movl $KVIRT_BASE, %eax          // set EAX with the kernel virtual base address
    shr $22, %eax                   // shift right by 22 bits to get the index of the kernel virtual base address in the page directory
    movl $PDE_SIZE, %ecx
    mull %ecx                       // multiple that index by the page directory entry size in order to get the offset from the page directory base
    addl $ENTRY_PD_BASE, %eax       // add that offset to the page directory base
    movl $PDE_FLAGS, (%eax)         // map the first 4MB of KVIRT_BASE to the first 4MB of physical memory [p=1; rw=1; pse=1]
  
    // // map the last 4MB of memory to the 4MB area in which the stack is in
    // mov eax, 0xFFFFFFFF >> 22   // set EAX with the index of the kenrel virtual base address in the page directory
    // mov ecx, PDE_SIZE
    // mul ecx                     // multiple that index by the page directory entry size in order to get the offset from the page directory base
    // add eax, ENTRY_PD_BASE      // add that offset to the page directory base
    // mov [eax], dword 0x83       // map the first 4MB of KVIRT_BASE to the first 4MB of physical memory [p=1; rw=1; pse=1]
    // or  [eax], 0x0008fffc & 0xFFF00000


    // enable Page Size Extension for 4MB pages and global pages
    movl %cr4, %eax
    orl $0x00000010, %eax
    orl $0x00000080, %eax
    movl %eax, %cr4

    // load the entry page directory address into cr3
    movl $ENTRY_PD_BASE, %eax
    movl %eax, %cr3

    // enable paging and write-protect by enabling bits 31 (PG) and 16 (WP) of cr0
    movl %cr0, %eax         // read current cr0
    orl $0x80000000, %eax   // [bit 31] set PG
    orl $0x00010000, %eax   // [bit 16] set WP
    mov %eax, %cr0          // update cr0

    ret