// Interrupt Service Routine :: Interrupt Handler // ~ eylon

#include <kernel/interrupts/isr.h>
#include <kernel/interrupts/pic.h>
#include <kernel/interrupts/idt.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <kernel/syscall.h>
#include <drivers/screen.h> // [DEBUG]

// Macro to extern and set an interrupt handler in the IDT
#define INTERRUPT_HANDLER(index, gate_type, dpl) extern void (interrupt_handler_##index)(); \
                                 set_interrupt_descriptor(index, (uint32_t) interrupt_handler_##index, gate_type, dpl);
// Array of interrupt handlers arranged by the interrupt index
static isr_t interrupt_handlers[IDT_ENTRIES_COUNT];


// Set a function handler to be called when the selected inerrupt occurs
void set_interrupt_handler(uint8_t index, isr_t func)
{
    interrupt_handlers[index] = func;
    // Unmask if it's an IRQ
    if (index >= IRQ0 && index <= IRQ15) { 
        unmask_irq(index-IRQ0);
    }
}

// Being called by the common_interrupt_handler :: call the interrupt's handler function and send an EOI if it's an IRQ.
void interrupt_handler(int_frame_t interrupt_data)
{
    /* !!! ANY CHANGE BEING DONE TO interrupt_data WILL AFFECT THE RESTORING OF THE USER CONTEXT !!! 
        This is required for returning values from syscalls. */

    // Call the associated interrupt handler
    if (interrupt_handlers[interrupt_data.interrupt_number] != NULL) {
        interrupt_handlers[interrupt_data.interrupt_number](&interrupt_data);
    }
    // Send an EOI signal to the PIC if the interrupt is an IRQ
    if (interrupt_data.interrupt_number >= IRQ0 && interrupt_data.interrupt_number <= IRQ15) {
        pic_eoi(interrupt_data.interrupt_number);
    }
    else kprint("NON PIC INTERRUPT", VGA_ATR_WARNING); // [TMP][DEBUG]
    return;
}

// Initiate interrupts :: initiate the PIC, fill the IDT, load the IDT, initiate the interrupt handlers array
void init_interrupt()
{
    __asm__ volatile ("cli");       // clear interrupt flag :: disable interrupts

    init_pic();                     // config and initiate the PIC :: all IRQs will be masked (except IRQ2)

    /* Set the IDT */
    INTERRUPT_HANDLER(0, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(1, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(2, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(3, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(4, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(5, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(6, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(7, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(8, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(9, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(10, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(11, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(12, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(13, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(14, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(15, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(16, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(17, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(18, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(19, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(20, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(21, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(22, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(23, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(24, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(25, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(26, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(27, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(28, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(29, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(30, GT_32BIT_TRAP, 0)
    INTERRUPT_HANDLER(31, GT_32BIT_TRAP, 0)

    INTERRUPT_HANDLER(32, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(33, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(34, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(35, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(36, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(37, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(38, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(39, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(40, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(41, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(42, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(43, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(44, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(45, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(46, GT_32BIT_INT, 0)
    INTERRUPT_HANDLER(47, GT_32BIT_INT, 0)

    INTERRUPT_HANDLER(92, GT_32BIT_TRAP, 3);    // syscall 0x5C

    load_idt();                                 // load the IDT into the PIC

    // Initiate the interrupt handlers array with NULL pointers
    for (int i = 0; i < IDT_ENTRIES_COUNT; i++) {
        interrupt_handlers[i] = NULL;
    }
}