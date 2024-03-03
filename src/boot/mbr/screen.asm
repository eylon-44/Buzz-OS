;; BIOS real mode screen utils ;; ~ eylon

;; Print a null terminated string from the [si] register
print:
    mov ah, 0x0E     ; bios teletype output function
    mov bh, 0x00     ; page number
    mov bl, 0x07     ; text attribute (white on black)

    print_loop_start:
        mov al, [si]         ; load the next character to be printed

        cmp al, 0            ; check for null terminator
        je print_loop_end    ; end the print if the character is null

        int 0x10             ; bios video services interrupt :: print the character

        inc si               ; move to the next character in the string
        jmp print_loop_start ; continue the loop

    print_loop_end:
    
    ;; Print new line
    mov al, 0x0D
    int 0x10
    mov al, 0x0A
    int 0x10

    ret             ; return


;; Clear the screen
clear_screen:
    ;; Clear screen
    mov ah, 0x07    ; bios scroll down function
    mov al, 0x00    ; number of lines to scroll down :: 0x00 = clear window
    mov bh, 0x07    ; set text color :: white on black
    mov cx, 0x0000  ; row = 0, col = 0
    mov dx, 0x184f  ; row = 24 (0x18), col = 79 (0x4f)
    int 0x10        ; bios video services interrupt

    ;; Reset cursor
    mov ah, 0x02   ; bios set cursor position function
    mov bh, 0x00   ; page number
    mov dh, 0x00   ; row
    mov dl, 0x00   ; column
    int 0x10       ; bios video services interrupt

    ret            ; return