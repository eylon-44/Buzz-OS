;; Detecting Available Physical Memory ;;

;;
;; We are going to use INT 0x15, EAX = 0xE820 for the task. For each call, this BIOS 
;; interrupt will write 20 bytes to ES:DI. The first 8 bytes represent the physical base 
;; address of the relevant region. The next 8 bytes represent the length of the region; 
;; the region should be ignore if this value is 0. The last 4 bytes represent the type of
;; the region as follows:
;;
;;  Type 1: Usable (normal) RAM
;;  Type 2: Reserved - unusable
;;  Type 3: ACPI reclaimable memory
;;  Type 4: ACPI NVS memory
;;  Type 5: Area containing bad memory
;;
;; Later, the kernel should only use "usable RAM" (type 1) and "ACPI reclaimable memory" (type 3).
;; We should also treat unlisted regions as "reserved" (type 2) and not use them.
;;
;; In order to use the service, we must follow these steps:
;;  - point ES:DI to the destination buffer
;;  - set EBX to 0
;;  - set EDX to the magic number 0x534D4150
;;  - set EAX to 0xE820
;;  - set ECX to 24
;;  - invoke an INT 0x15
;;
;; If the first call to the function is successful, EAX will be set to 0x534D4150, and the
;; Carry flag will be clear. EBX will be set to some non-zero value, which must be preserved 
;; for the next call to the function. CL will contain the number of bytes stored at ES:DI (probably 20).
;; Now repeat:
;;  - increment DI by the list entry size stored in CL 
;;  - reset EAX to 0xE820
;;  - reset ECX to 24
;;  - invoke an INT 0x15
;;
;; When you reach the end of the list, EBX is set to 0. If you call the function again with EBX = 0, 
;; the list will start over.
;;
;; In order to supply the kernel a comfortable and reliable interface for reading the data, we are going 
;; to define the first entry as follows:
;;  - the first 4 bytes will hold the magic number 0x534D4150
;;  - the next 4 bytes will be set to the number of entries in the list not including this entry
;;  - the next 4 bytes will be set to the size of a single entry (probably 20)
;;  - each of the last 8 bytes will be set to 0xff

%define BUFF_BASE  0x8000       ; buffer start address
%define MAGIC      0x534D4150   ; INT 0x15 magic number
%define ENTRY_SIZE 20           ; assuming entry size is 20 bytes

mm_detect:
    mov di, word BUFF_BASE      ; set DI to the starting address of the buffer in which the INT 0x15 data will be loaded
    mov edx, MAGIC              ; set EDX with the INT 0x15 magic number
    xor ebx, ebx                ; set EBX to 0
    xor cl, cl                  ; set CL to 0 in order for it to not affect the [.detect] loop in its first iteration

    ;; make the kernel's meta data entry (20 bytes located at the buffer base)
    mov [BUFF_BASE], dword MAGIC            ; [base_low]  set the first 4 bytes with the magic number
    mov [BUFF_BASE+4], dword 0              ; [base_high] initiate the next 4 bytes with 0, they will later indicate the number of entries in the list 
    mov [BUFF_BASE+8], dword ENTRY_SIZE     ; [size_low]  initiate the next 4 bytes with the size of an entry in the list
    mov [BUFF_BASE+12], dword 0xFFFFFFFF    ; [size_high] 0xFF
    mov [BUFF_BASE+16], dword 0xFFFFFFFF    ; [type]      0xFF
    
    add di, word ENTRY_SIZE                 ; increament DI to point to the next entry

    .detect:
        add di, cx              ; increament DI by CL (the size of the last entry)
        mov eax, 0xE820         ; set EAX with the constant 0xE820
        mov ecx, 24             ; set CL with the constant 24
        int 0x15                ; invoke BIOS INT 0x15


        jc .err                     ; error if the carry flag is set
        cmp eax, MAGIC
        jne .err                    ; error if EAX is not equal to the magic
        
        add [BUFF_BASE+4], dword 1  ; increament the number of entries attribue in the meta data entry

        test ebx, ebx
        jnz .detect         ; loop until EBX is equal to 0, meaning the end of the regions list

    ;; Success, return
    ret

    ;; Error, loop forever
    .err:
        mov si, ERR_MM_DETECT
        call print              ; {screen.asm} print string
        jmp $