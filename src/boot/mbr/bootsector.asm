;; Buzz OS Bootsector ;; ~ eylon

[org 0x7C00]
[bits 16]

mov [BOOT_DRIVE], dl ; the bios sets up the boot drive in [dl] on boot, saving it for later use

BOOT_START:
    cli                 ; clear interrupt flag :: disable maskable interrupts, we can't handle them yet

    ;; Zero out data segments registers DS, ES and SS and set SP
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ;; [TODO]? A20 line

    call clear_screen       ; {screen.asm} clear the screen
    mov si, MSG_WECLOME
    call print              ; {screen.asm} print string

    call mm_detect          ; {mm_detect.asm} detect physical available memory

    call load_bootloader    ; {disk.asm} load the second stage bootloader into memory

    call switch_to_pm       ; {protected_mode.asm} switch to protected mode. does not return, instead jumps to [PM_START]

    jmp $                   ; this code should never run

[bits 32]
PM_START:
    call BOOTLOADER_OFFSET      ; jump to the loaded bootloader location
    jmp $                       ; this code should never run

;: Include
%include "protected_mode.asm"
[bits 16]
%include "screen.asm"
%include "mm_detect.asm"
%include "disk.asm"
%include "gdt.asm"

;; Global Variables
BOOT_DRIVE: db 0                 ; storing the used boot drive in memory because [dl] may get overwritten
BOOTLOADER_OFFSET: equ 0x1000    ; address to load the bootloader in memory
BOOTLOADER_SIZE:   equ 20         ; bootloader size in sectors

MSG_WECLOME:    db "WELCOME! LOADING BUZZ OS . . .", 0xA, 0
ERR_MM_DETECT:  db "ERROR: FAILED TO DETECT MEMORY", 0XA, 0
ERR_DISK:       db "ERROR: DISK READ ERROR", 0xA, 0
ERR_SECTORS:    db "ERROR: INCORRECT NUMBER OF SECTORS READ", 0xA, 0

times 510-($-$$) db 0   ; padding to 512 bytes
dw 0xAA55               ; boot signature