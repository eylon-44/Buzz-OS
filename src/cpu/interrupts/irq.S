// Interrupt Request Handler // ~ eylon

.section .text
.extern interrupt_handler
.extern queue

/* The CPU does not push the interrupt number so we will have to do it ourselves.
    Define for each interrupt an interrupt_handler_[X] function, where [X] is the interrupt number.
    Each of these functions will be set in the IDT with its corresponding interrupt number.
    If this is a non error interrupt, the function will push 0 as the error code to keep a constant struct size.
    The function will push the interrupt number onto the stack and call the common_interrupt_handler to continue from there.
    These values will later be passed to the C function to interpret them and call the corresponding C handler. */
.macro no_error_code_interrupt_handler inum
    .global interrupt_handler_\inum
    interrupt_handler_\inum:            // define the interrupt handler function
        cli                             // disable maskable interrupts
        pushl $0                        // push 0 as error code
        pushl $\inum                    // push the interrupt number
        jmp common_interrupt_handler    // jump to the common interrupt handler
.endm

.macro error_code_interrupt_handler inum
    .global interrupt_handler_\inum
    interrupt_handler_\inum:            // define the interrupt handler function
        cli                             // disable maskable interrupts
        pushl $\inum                    // push the interrupt number
        jmp common_interrupt_handler    // jump to the common interrupt handler
.endm


/* The common parts of the generic interrupt handler.
    Save all registers in order to save the state of the interrupted process the just stopped.
    Call the C interrupt_handler to handle the interrupt.
    Restore the interrupted process, restore the stack to the state as the cpu gave it to us. 
    Return to the interrupted code. */
common_interrupt_handler:
    /* The sched_queue_t and thread_node_t types are defined in scheduler.h as follows:
            typedef struct
            {
                thread_node_t* active;  // currently active thread from the linked list

                ...
            } sched_queue_t;

            struct thread_node {
                thread_t thread;        // current
                thread_node_t* next;    // next
            };

        And the thread_t type is defined in pm.h as described below:
            struct thread {
                size_t cr3;             // CR3 register - address space
                size_t kesp;            // kernel stack pointer

                ...
            };

        Based on that we can access [cr3] and [kesp] of the [active] thread as so:
            movl queue+0, %eax     
            movl 0(%eax), %ebx      // EBX=cr3
            movl 4(%eax), %ecx      // ECX=kesp
        
*/


    /*[TODO] delete */
    // /* Save ESP in queue.active->thread.kesp. We must use a register to access the pointer
    //     because x86 won't allow double refrencing, but we are not allowed to override any
    //     register because we haven't saved them yet (using pushal). We can save EAX in the
    //     stack without changing ESP and later restore it. */

    // movl %eax, -4(%esp)     // save EAX in the stack

    // mov queue+0, %eax       // EAX = queue.active
    // movl %esp, 4(%eax)      // queue.active->thread.kesp = ESP
    
    // movl -4(%esp), %eax     // restore EAX from the stack
    

    // Handle interrupt
    pushal                  // save all registers :: save the state of the interrupted process

    // Save ESP in for the active thread (queue.active->thread.kesp)
    mov queue+0, %eax       // EAX = queue.active
    movl %esp, 4(%eax)      // queue.active->thread.kesp = ESP

    call interrupt_handler  // call the C function to handle the interrupt

    // Perform a task switch (load a different page directory and ESP) if necessary
    movl queue+0, %eax      // EAX = queue.active
    movl 0(%eax), %ebx      // EBX = queue.active->thread.cr3
    movl 4(%eax), %ecx      // ECX = queue.active->thread.kesp
    movl %cr3, %edx         // EDX = CR3

    cmpl %ebx, %edx         // if (queue.active->thread.cr3 == CR3) {
    je .return              //  return;
                            // }
                            // else {
    movl %ebx, %cr3         //  CR3 = switch_to.cr3
    movl %ecx, %esp         //  ESP = switch_to.kesp
                            // }

    // Return to user space
    .return:
        popal               // restore the registers :: restore the state of the interrupted process
        addl $8, %esp       // iret expectes the stack to be the same as the time of the interrupt
        sti                 // enable interrupts
        iretl               // return to the code that got interrupted



// Set all interrupt request handlers.
// https://wiki.osdev.org/Interrupts#General_IBM-PC_Compatible_Interrupt_Information
// https://wiki.osdev.org/Exceptions

// CPU Exceptions
no_error_code_interrupt_handler 0          // [fault]      devision by 0 error
no_error_code_interrupt_handler 1          // [fault/trap] debug exception
no_error_code_interrupt_handler 2          // [interrupt]  non maskable interrupt
no_error_code_interrupt_handler 3          // [trap]       breakpoint

no_error_code_interrupt_handler 4          // [trap]       overflow
no_error_code_interrupt_handler 5          // [fault]      bound range exceeded
no_error_code_interrupt_handler 6          // [fault]      invalid opcode
no_error_code_interrupt_handler 7          // [fault]      device not available

error_code_interrupt_handler    8          // [abort]*     double float
no_error_code_interrupt_handler 9          // [fault]      coprocessor segment overrun
error_code_interrupt_handler    10         // [fault]*     invalid tss
error_code_interrupt_handler    11         // [fault]*     segment not present

error_code_interrupt_handler    12         // [fault]*     stack segment fault
error_code_interrupt_handler    13         // [fault]*     general protection fault
error_code_interrupt_handler    14         // [fault]*     page fault
no_error_code_interrupt_handler 15         // [reserved]

no_error_code_interrupt_handler 16         // [fault]      x87 floating point exception
error_code_interrupt_handler    17         // [fault]*     alignment check
no_error_code_interrupt_handler 18         // [abort]      machine check
no_error_code_interrupt_handler 19         // [fault]      SIMD floating-point exception

no_error_code_interrupt_handler 20         // [fault]      virtualization exception
error_code_interrupt_handler    21         // [fault]*     control protection exception
no_error_code_interrupt_handler 22         // [reserved]
no_error_code_interrupt_handler 23         // [reserved]

no_error_code_interrupt_handler 24         // [reserved]
no_error_code_interrupt_handler 25         // [reserved]
no_error_code_interrupt_handler 26         // [reserved]
no_error_code_interrupt_handler 27         // [reserved]

no_error_code_interrupt_handler 28         // [fault]      hypervisor injection exception
error_code_interrupt_handler    29         // [fault]*     VMM communication exception
error_code_interrupt_handler    30         // [fault]*     secutiry exception
no_error_code_interrupt_handler 31         // [reserved]


// Master PIC
no_error_code_interrupt_handler 32         // [IRQ0]       programable interrupt timer interrupt
no_error_code_interrupt_handler 33         // [IRQ1]       keyboard interrupt
no_error_code_interrupt_handler 34         // [IRQ2]       cascade PIC2
no_error_code_interrupt_handler 35         // [IRQ3]       COM2

no_error_code_interrupt_handler 36         // [IRQ4]       COM1
no_error_code_interrupt_handler 37         // [IRQ5]       LPT2
no_error_code_interrupt_handler 38         // [IRQ6]       floppy disk
no_error_code_interrupt_handler 39         // [IRQ7]       LPT1 / unreliable "spurious" interrupt (usually)

// Slave PIC
no_error_code_interrupt_handler 40         // [IRQ8]       CMOS real-time clock
no_error_code_interrupt_handler 41         // [IRQ9]       free for peripherals / legacy SCSI / NIC
no_error_code_interrupt_handler 42         // [IRQ10]      free for peripherals / legacy SCSI / NIC
no_error_code_interrupt_handler 43         // [IRQ11]      free for peripherals / legacy SCSI / NIC

no_error_code_interrupt_handler 44         // [IRQ12]      PS2 mouse
no_error_code_interrupt_handler 45         // [IRQ13]      FPU / coprocessor / inter-processor
no_error_code_interrupt_handler 46         // [IRQ14]      primary ATA hard disk
no_error_code_interrupt_handler 47         // [IRQ15]      secondary ATA hard disk

// Custom
no_error_code_interrupt_handler 92         // [syscall]    interrupt performed by the user to invoke a syscall