;; Bootstrap GDT ;; ~ eylon
;; The Global Descriptor Table (GDT) contains Segment Descriptors (SD), each 8 bytes that describes the segment properties ;;

;; GDT Segment Descriptor Macro ;;
;; @base [32 bits]
;; @limit [20 bits]
;; @access-byte [8 bits]
;; > flags [4 bits] is set by default to 0xC0: Granularity and 32 bit segment.
;; > access-byte [8 bits], flags Present[7] and Descriptor-Type[4] are enabled and DPL[5-6] is set to ring 0 by default
%macro GDT_SEG 3
    ;; limit [0-15]   base [16-31]
    dw ((%2) & 0xffff), ((%1) & 0xffff)
    ;; base [32-39]            access byte [40-47]
    db (((%1) >> 16) & 0xff), ((0x90 | (%3)) & 0xff)
    ;; flags[52-55] limit [48-51]      base [56-63]
    db (0xC0 | (((%2) >> 16) & 0xf)), (((%1) >> 24) & 0xff)
%endmacro

;; Segment access-byte flags
%define SEG_DATA       0b00000000 ; [3]      the descriptor defines a data segment
%define SEG_EXECUTABLE 0b00001000 ; [3]      the descriptor defines an executable code segment
%define SEG_XREADABLE  0b00000010 ; [1]      for code segments only :: read access is allowed
%define SEG_DWRITEABLE 0b00000010 ; [1]      for data segments only :: write access is allowed
%define SEG_ACCESSED   0b00000001 ; [0]      the cpu will set the accessed bit when the segment is accessed unless set to 1 in advance


;; Setting a minial Bootsector GDT
gdt:
    ;; The GDT starts with a null segment descriptor :: 8 null bytes
    null_descriptor:
        dd 0x0 ; null double word
        dd 0x0 ; null double word

    ;; Code segment descriptor
    code_descriptor:
        GDT_SEG 0, 0xfffff, (SEG_EXECUTABLE | SEG_XREADABLE)

    ;; Data segment descriptor
    data_descriptor:
        GDT_SEG 0, 0xfffff, (SEG_DATA | SEG_DWRITEABLE)


;; GDT descriptor :: GDT size [16 bits], GDT address [32 bits]
gdt_descriptor:
    dw $ - gdt - 1 ; size of the GDT, always less 1 of the true size
    dd gdt                     ; GDT start address


; Define some handy constants for the GDT segment descriptor offsets, which
; are what segment registers must contain when in protected mode. For example,
; when we set DS=0x10 in PM, the CPU knows that we mean it to use the
; segment described at offset 0x10 (16 bytes) in our GDT, which in our
; case is the DATA segment (0x0 -> NULL :: 0x08 -> CODE :: 0x10 -> DATA)
CODE_SEG equ code_descriptor - gdt
DATA_SEG equ data_descriptor - gdt