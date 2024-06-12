;; BIOS real mode disk utils ;; ~ eylon

;; Read [BOOTLOADER_SIZE] sectors from disk and load them into memory at [BOOTLOADER_OFFSET]
load_bootmain:
    mov ah, 0x02                ; bios read sectors into memory function
    mov ch, 0x00                ; track number
    mov cl, 0x02                ; sector number :: where the start of the bootloader is placed
    mov dh, 0x00                ; head number
    mov dl, [BOOT_DRIVE]        ; drive number
    mov al, BOOTLOADER_SIZE     ; nuber of sectors to read
    mov bx, BOOTLOADER_OFFSET   ; read from disk and store in [BOOTLOADER_OFFSET](=0x1000)

    int 0x13                    ; bios disk services interrupt

    ;; Check for errors
    jc disk_error
    cmp ah, 0
    jne sectors_error
    ret

disk_error:
    mov si, ERR_DISK
    call print
    jmp disk_loop

sectors_error:
    mov bx, ERR_SECTORS
    call print

disk_loop:
    jmp $