;; Kernel Entry ;; ~ eylon

[bits 32]
global _start
extern kernel_main
extern gdt_descriptor
extern proc_pd

;; Kernel's virtual and physical bases are used to convert virtual addresses to physical ones
%define KVIRT_BASE 0xC0100000
%define KPHYS_BASE 0x100000

;; Convert virtual address to physical
%define V2P(vaddr)  ((vaddr) - (KVIRT_BASE - KPHYS_BASE))

;; Scratch space to place the entry page direcotry in
%define PD_BASE   0x10000
;; PDE count in a single PD
%define PDE_COUNT 1024
;; PDE size in bytes
%define PDE_SIZE  4

;; Physical address of [_entry]; the bootloader should jump to this address because paging is still off
_start: equ V2P(_entry)

;; Kernel entry point
_entry:
    cli                     ; disable interrupts
    call enable_paging
    call setup_gdt

    ;; set the stack at the higher half kernel
    mov ebp, 0xc0090000
    mov esp, ebp

    ;; jump to the kernel's main function in the higher half
    ;; use an indirect call because the assembler produces a PC-relative instruction for a direct jump
    mov eax, kernel_main
	call eax

    ;; This code should next run
    jmp $


;; Setup the GDT
setup_gdt:
    ; load the gdt
    lgdt [gdt_descriptor]
    ret

;; Enable paging and set a basic startup page directory
enable_paging:
    ;; enable Page Size Extension for 4MB pages
    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax


    ;; generate entry page directory; map first 4MB of virtual address 0 and 3GB to physical address 0

    ;; zero out all PDEs
    mov ebx, PD_BASE            ; page direcotry base address
    mov ecx, PDE_COUNT          ; number of page directory entries in a single page directory

    .loop_pd:
        mov [ebx], dword 0      ; zero out the value pointed by EBX
        add ebx, PDE_SIZE       ; increase EBX's size by the size of a page directory entry in order to point the next one
        loop .loop_pd           ; loop

    ;; identity map first 4MB of memory [v: 0-4MB = p: 0-4MB]
    mov ebx, PD_BASE            ; set EBX with the page directory base address
    mov [ebx], dword 0x83       ; identity map first 4MB of memory [p=1; rw=1; pse=1]

    ;; map first 4MB of virtual 3GB to start of physical [v: 3GB-3GB+4MB = p: 0-4MB]
    mov eax, KVIRT_BASE >> 22   ; set EAX with the index of the kenrel virtual base address in the page directory
    mov ecx, PDE_SIZE
    mul ecx                     ; multiple that index by the page directory entry size in order to get the offset from the page directory base
    add eax, PD_BASE            ; add that offset to the page directory base
    mov [eax], dword 0x83       ; map the first 4MB of KVIRT_BASE to the first 4MB of physical memory [p=1; rw=1; pse=1]
  

    ;; load the startup Page Directory address into cr3
    mov eax, PD_BASE
    mov cr3, eax


    ;; enable paging and write-protect by enabling bits 31 (PG) and 16 (WP) of cr0
    mov eax, cr0        ; read current cr0
    or eax, 0x80000000  ; [bit 31] set PG
    or eax, 0x00010000  ; [bit 16] set WP
    mov cr0, eax        ; update cr0

    ret