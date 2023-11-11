;; Buzz OS Bootsector ;; ~ eylon

[org 0x7c00]
[bits 16]

mov [BOOT_DRIVE], dl ; the bios sets up the boot drive in [dl] on boot, saving it for later use

BOOT_START:
    cli                 ; clear interrupt flag :: disable maskable interrupts, we can't handle them yet

    ;; Zero data segments registers DS, ES and SS and set SP
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ;; [TODO]? A20 line

    call clear_screen   ; {screen.asm} clear the screen
    mov si, MSG_BOOT_STRT
    call print          ; {screen.asm} print string

    call load_kernel    ; {disk.asm} load the kernel into memory 

    mov si, MSG_PROT_MODE
    call print          ; {screen.asm} print string

    call switch_to_pm   ; {protected_mode.asm} switch to protected mode. will not return to here but will jump to [PM_START]

    jmp $               ; should never reach this instruction if everything goes well

[bits 32]
PM_START:
    call KERNEL_OFFSET  ; jump to the kernel's entry in physical memory
    jmp $               ; stay here if the kernel returns, it should never do

;: Utils
%include "protected_mode.asm"
[bits 16]
%include "screen.asm"
%include "disk.asm"
%include "gdt.asm"

;; Global Variables
BOOT_DRIVE: db 0             ; storing the used boot drive in memory because [dl] may get overwritten
KERNEL_OFFSET equ 0x1000     ; kernel offset in memory

MSG_BOOT_STRT: db "[16-bit real mode]", 0x0A, 0x0D, "> Starting bootsector", 0x0A, 0x0D
MSG_KRNL_LOAD: db "> Loding kernel from disk",                                              0
MSG_PROT_MODE: db "> Switching to protected mode", 0x0A, 0x0D, "[32-bit protected mode]",   0
MSG_PANIC:     db "Panic!",                                                                 0

times 510-($-$$) db 0   ; padding to 512 bytes
dw 0xAA55               ; boot signature