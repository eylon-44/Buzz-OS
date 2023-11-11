;; Switching to protected mode ;; ~ eylon

[bits 16]
switch_to_pm:
    lgdt [gdt_descriptor] ; load the bootstrap global descriptor table :: identity mapped

    ;; setting the first bit of the cr0 control register to 1 in order to switch to protected mode
    mov eax, cr0
    or  eax, 0x1
    mov cr0, eax
    ;; we are now in protected mode

    jmp CODE_SEG:pm_init ; jump far to the 32-bit code segment, leaving real mode for good

[bits 32]
;; Initialise registers and the stack once in PM
pm_init:
    ;; Now in protected mode the old segments are invalid, 
    ;; so we point our segment registers to the data segment we defined in the GDT
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;; Update the stack position to a high place in memory
    mov ebp, 0x9000
    mov esp, ebp

    ;; Start a protected mode routine
    jmp PM_START