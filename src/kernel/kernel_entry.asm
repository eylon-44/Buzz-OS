;; Kernel Entry ;; ~ eylon

[bits 32]
global _start
extern kernel_main
extern gdt_descriptor
extern proc_pd

;; Kernel's virtual and physical bases are used to convert virtual addresses to physical ones
%define KVIRT_BASE 0xC0000000
%define KPHYS_BASE 0x100000

;; Convert virtual address to physical
%define V2P(vaddr)  ((vaddr) - (KVIRT_BASE - KPHYS_BASE))

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
	jmp eax


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

    ;; load the startup Page Directory address into cr3
    mov eax, V2P(proc_pd)
    mov cr3, eax

    ;; enable paging and write-protect by enabling bits 31 (PG) and 16 (WP) of cr0
    mov eax, cr0        ; read current cr0
    or eax, 0x80000000  ; [bit 31] set PG
    or eax, 0x00010000  ; [bit 16] set WP
    mov cr0, eax        ; update cr0

    ret