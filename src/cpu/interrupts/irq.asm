;; Interrupt Request Handler ;; ~ eylon

[extern interrupt_handler]

;; The CPU does not push the interrupt number so we will have to do it ourselves.
;; Define for each interrupt an interrupt_handler_[X] function, where [X] is the interrupt number.
;; Each of these functions will be set in the IDT with its corresponding interrupt number.
;; If this is a non error interrupt, the function will push 0 as the error code to keep a constant struct size.
;; The function will push the interrupt number onto the stack and call the common_interrupt_handler to continue from there.
;; These values will later be passed to the C function to interpret them and call the corresponding C handler.
%macro no_error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:               ; define the interrupt handler function
        push dword 0                     ; push 0 as error code
        push dword %1                    ; push the interrupt number
        jmp common_interrupt_handler    ; jump to the common interrupt handler
%endmacro

%macro error_code_interrupt_handler 1
    global interrupt_handler_%1
    interrupt_handler_%1:               ; define the interrupt handler function
        push dword %1                   ; push the interrupt number
        jmp common_interrupt_handler    ; jump to the common interrupt handler
%endmacro


;; The common parts of the generic interrupt handler.
;; Save all registers in order to save the state of the interrupted process the just stopped.
;; Call the C interrupt_handler to handle the interrupt.
;; Restore the interrupted process, restore the stack to the state as the cpu gave it to us. 
;; Return to the interrupted code.
common_interrupt_handler:
    pusha                   ; save all registers :: save the state of the interrupted process
    call interrupt_handler  ; call the C function to handle the interrupt
    popa                    ; restore the registers :: restore the state of the interrupted process
    add esp, 8              ; iret expectes the stack should to be the same as the time of the interrupt
    iret                    ; return to the code that got interrupted


;; Set all interrupt request handlers.
;; The error code interrupts are interrupts number 7, 9, 10, 11, 12, 13 and 16.

;; Master PIC
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3

no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7

;; Slave PIC
no_error_code_interrupt_handler 8
no_error_code_interrupt_handler 9
no_error_code_interrupt_handler 10
no_error_code_interrupt_handler 11

no_error_code_interrupt_handler 12
no_error_code_interrupt_handler 13
no_error_code_interrupt_handler 14
no_error_code_interrupt_handler 15