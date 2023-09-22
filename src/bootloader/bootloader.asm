bits 16         ; set 16 bit processor mode :: works best with bios
org 0x7c00      ; set program starting point in memory to 0x7c00 :: where the bios jumps to

start: jmp boot

;; Variables
wlcmsg db "- Welcome to Buzz OS. -"
wlclen equ $-wlcmsg

;; Functions
clear_screen:
    mov ah, 0x07    ; scroll down function
    mov al, 0x00    ; number of lines to scroll down :: 0x00 = clear window
    mov bh, 0x07    ; set text color :: white on black
    mov cx, 0x0000  ; row = 0, col = 0
    mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f)
    int 0x10        ; bios video services interrupt

    ret             ; return

;; Hide cursor and position it in the middle of the screen
set_cursor:
    ;; Hide cursor
    mov ah, 0x01    ; set cursor size function
    mov cx, 0x2607  ; set ch and cl :: cursor starting and ending scan line :: invisible cursor
    int 0x10        ; call bios video interrupt

    ;; Find middle of screen
    mov ah, 0x0F    ; set get video mode function
    int 0x10        ; call bios video interrupt
    
    mov bh, al      ; copy the number of rows to bh
    mov bl, ah      ; copy the number of columns to bl
    sub bl, wlclen  ; subtract string length from screen size
    shr bh, 1       ; shit right by one :: devide by two
    shr bl, 1       ; shit right by one :: devide by two

    ;; Set cursor
    mov ah, 0x02    ; set cursor function
    mov bh, 0x00    ; page number
    mov dh, 0x02    ; cursor row
    mov dl, bl      ; cursor column
    int 0x10        ; bios video services interrupt

    ret

print_welcome:
    mov ah, 0x13    ; write string function
    mov al, 0x01    ; write mode :: move cursor
    mov bh, 0x00    ; page number
    mov bl, 0x07    ; text color :: white on balck
    mov cx, wlclen  ; string length
    mov bp, wlcmsg  ; pointer to message
    int 0x10        ; bios video services interrupt

    ret

;; P.204
read_disk:
    mov ah, 0x02    ; read disk function

    ;; set buffer pointer :: (es:bx) = pointer to buffer, the starting address of the buffer
    mov ax, 0x00 
    mov es, ax 
    mov bx, 0x1000    

    mov al, 0x02    ; number of sectors to read
    mov ch, 0x00    ; track number
    mov cl, 0x02    ; sector number to read from
    mov dh, 0x00    ; head number
    mov dl, 0x00    ; drive number

    int 0x13        ; bios disk services interrupt


    ; Check for errors (AH should be 0)
    jc disk_error
    cmp ah, 0
    jnz disk_error

    jmp 0x0000:0x1000
    
    ret

disk_error:
    ; Handle disk read error here (e.g., print an error message)
    ; Infinite loop to halt execution
    ; Display an error message with the AH register (error code)
    mov ah, 0x0E     ; BIOS teletype output function
    mov al, 'E'      ; Print 'E' for Error
    int 0x10         ; Call BIOS interrupt to print 'E'

    mov ah, 0x0E     ; BIOS teletype output function
    mov al, '0'      ; Print the tens digit of the error code
    add al, ah       ; Convert the error code to ASCII
    int 0x10         ; Call BIOS interrupt to print tens digit

    mov al, '0'      ; Print the ones digit of the error code
    mov ah, 0x0E     ; BIOS teletype output function
    adc al, 0        ; Convert the error code to ASCII
    int 0x10         ; Call BIOS interrupt to print ones digit

    ; Infinite loop to halt execution
    jmp $

boot:
    cli     ; clear interrupt flag :: disable maskable interrupts
    cld     ; clear direction flag

    ; Print a welcome message
    call clear_screen
    call set_cursor
    call print_welcome

    ;; Load kernel from disk
    call read_disk

    hlt     ; halt the machine

times 510 - ($-$$) db 0     ; set unused memory to 0 until the boot signiture
dw 0xAA55                   ; boot signiture 


;; Todo :: Fix can't get how many rows on the screen
;; Todo :: Can't read disk when segment (es) value is not 0x00