;; Kernel Entry ;; ~ eylon

[bits 32]
global _start
extern kernel_main
extern page_directory

;; Kernel virtual offset :: use this to convert virtual to physical addresses
%define KERNEL_VIRTUAL 0xC0000000

_start:

	;; Enable Page Size Extension for 4MB pages
    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax

    ;; Load the Page Directory address into cr3
    mov eax, page_directory - KERNEL_VIRTUAL
    mov cr3, eax

    ;; Enable paging by setting bit 31 (PG/paging-enable bit) of cr0 to 1.
    mov eax, cr0        ; read current cr0
    or eax, 0x80000000  ; set PG
    mov cr0, eax        ; update cr0

    ;; Set the stack at the higher half kernel
    mov ebp, 0xc0090000
    mov esp, ebp

    ;; Jump to the kernel's main function in the higher half
    ;; Use an indirect call because the assembler produces a PC-relative instruction for a direct jump
    mov eax, kernel_main
	jmp eax