;; Global descriptor table ;; ~ eylon
;; The Global Descriptor Table (GDT) contains Segment Descriptors (SD), each 8 bytes that describes the segment properties ;;

gdt_start:
    ;; the GDT starts with a null SD :: 8 null bytes
    null_descriptor:
        dd 0x0 ; null double word
        dd 0x0 ; null double word

    ;; code SD
    code_descriptor:
        ; base=0x0, limit=0xfffff,
        ; 1st flags   : (present) 1 (privilege) 00 (segment type) 1         -> 1001b
        ; type flags  : (code) 1 (conforming) 0 (readable) 1 (accessed) 0   -> 1010b
        ; other flags : (granularity) 1 (32-bits) 1 (64-bits seg) 0 (AVL) 0 -> 1100b

        dw 0xffff      ; limit (0-15)
        dw 0x0         ; base  (0-15)
        db 0x0         ; base  (16-23)
        db 10011010b   ; flags (8 bits)
        db 11001111b   ; flags (4 bits) + segment length (16-19)
        db 0x0         ; base  (24-31)
        
    ;; data SD
    data_descriptor:
        ; same as the [code_descriptor] except for the type flags:
        ; type flags : (code) 0 (expand down) 0 (writable) 1 (accessed)0 -> 0010 b
        dw 0xffff      ; limit (0-15)
        dw 0x0         ; base  (0-15)
        db 0x0         ; base  (16-23)
        db 10010010b   ; flags (8 bits)
        db 11001111b   ; flags (4 bits) + segment length (16-19)
        db 0x0         ; base  (24-31)

gdt_end:

;; GDT descriptor :: GDT size [16 bits], GDT address [32 bits]
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of the GDT, always less 1 of the true size
    dd gdt_start               ; GDT start address


; Define some handy constants for the GDT segment descriptor offsets, which
; are what segment registers must contain when in protected mode. For example,
; when we set DS=0x10 in PM, the CPU knows that we mean it to use the
; segment described at offset 0x10 (16 bytes) in our GDT, which in our
; case is the DATA segment (0x0 -> NULL :: 0x08 -> CODE :: 0x10 -> DATA)
CODE_SEG equ code_descriptor - gdt_start
DATA_SEG equ data_descriptor - gdt_start