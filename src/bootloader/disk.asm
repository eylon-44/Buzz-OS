;; BIOS real mode disk utils ;; ~ eylon

;; Read disk and load into memory at [KERNEL_OFFSET]
load_kernel:
    mov ah, 0x02          ; bios read sectors into memory function
    mov ch, 0x00          ; track number
    mov cl, 0x02          ; sector number :: where the start of the kernel will be placed
    mov dh, 0x00          ; head number
    mov dl, [BOOT_DRIVE]  ; drive number
    mov al, 0x16          ; nuber of sectors to read :: TODO! increase the number of sectors to read as kernel grow in size
    mov bx, KERNEL_OFFSET ; read from disk and store in [KERNEL_OFFSET](=0x1000)

    int 0x13              ; bios disk services interrupt

    ;; Check for errors
    jc disk_error
    cmp ah, 0
    jne sectors_error
    ret

disk_error:
    mov si, DISK_ERROR
    call print
    jmp disk_loop

sectors_error:
    mov bx, SECTORS_ERROR
    call print

disk_loop:
    jmp $

DISK_ERROR:    db "Disk read error", 0
SECTORS_ERROR: db "Incorrect number of sectors read", 0