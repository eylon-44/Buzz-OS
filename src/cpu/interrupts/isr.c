// Interrupt Service Routine :: Interrupt Handler // ~ eylon

#include <cpu/interrupts/isr.h>
#include <cpu/interrupts/pic.h>
#include <cpu/interrupts/idt.h>
#include <utils/type.h>
#include <drivers/screen.h> // [DEBUG]

// Macro to extern and set an interrupt handler in the IDT
#define INTERRUPT_HANDLER(index) extern void (interrupt_handler_##index)(); \
                                 set_interrupt_descriptor(index, (u32_t) interrupt_handler_##index);
// Array of interrupt handlers arranged by the interrupt index
isr_t interrupt_handlers[IDT_ENTRIES_COUNT];


// Set a function handler to be called when the selected inerrupt occurs
void set_interrupt_handler(u8_t index, isr_t func)
{
    interrupt_handlers[index] = func;
    // unmask if it's an IRQ
    if (index >= IRQ0 && index <= IRQ15) { 
        unmask_irq(index-IRQ0);
    }
}

// Being called by the common_interrupt_handler :: call the interrupt's handler function and send an EOI if it's an IRQ.
void interrupt_handler(const InterruptData interrupt_data)
{
    // call the associated interrupt handler
    if (interrupt_handlers[interrupt_data.interrupt_number] != NULL) {
        interrupt_handlers[interrupt_data.interrupt_number]();
    }
    // send an EOI signal to the PIC if the interrupt is an IRQ
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

    // set the IDT
    INTERRUPT_HANDLER(0)
    INTERRUPT_HANDLER(1)
    INTERRUPT_HANDLER(2)
    INTERRUPT_HANDLER(3)
    INTERRUPT_HANDLER(4)
    INTERRUPT_HANDLER(5)
    INTERRUPT_HANDLER(6)
    INTERRUPT_HANDLER(7)
    INTERRUPT_HANDLER(8)
    INTERRUPT_HANDLER(9)
    INTERRUPT_HANDLER(10)
    INTERRUPT_HANDLER(11)
    INTERRUPT_HANDLER(12)
    INTERRUPT_HANDLER(13)
    INTERRUPT_HANDLER(14)
    INTERRUPT_HANDLER(15)
    INTERRUPT_HANDLER(16)
    INTERRUPT_HANDLER(17)
    INTERRUPT_HANDLER(18)
    INTERRUPT_HANDLER(19)
    INTERRUPT_HANDLER(20)
    INTERRUPT_HANDLER(21)
    INTERRUPT_HANDLER(22)
    INTERRUPT_HANDLER(23)
    INTERRUPT_HANDLER(24)
    INTERRUPT_HANDLER(25)
    INTERRUPT_HANDLER(26)
    INTERRUPT_HANDLER(27)
    INTERRUPT_HANDLER(28)
    INTERRUPT_HANDLER(29)
    INTERRUPT_HANDLER(30)
    INTERRUPT_HANDLER(31)
    INTERRUPT_HANDLER(32)
    INTERRUPT_HANDLER(33)
    INTERRUPT_HANDLER(34)
    INTERRUPT_HANDLER(35)
    INTERRUPT_HANDLER(36)
    INTERRUPT_HANDLER(37)
    INTERRUPT_HANDLER(38)
    INTERRUPT_HANDLER(39)
    INTERRUPT_HANDLER(40)
    INTERRUPT_HANDLER(41)
    INTERRUPT_HANDLER(42)
    INTERRUPT_HANDLER(43)
    INTERRUPT_HANDLER(44)
    INTERRUPT_HANDLER(45)
    INTERRUPT_HANDLER(46)
    INTERRUPT_HANDLER(47)

    load_idt();                     // load the IDT into the PIC

    // initiate the interrupt handlers array with NULL pointers
    for (int i = 0; i < IDT_ENTRIES_COUNT; i++) {
        interrupt_handlers[i] = NULL;
    }

    __asm__ volatile ("sti");       // set interrupt flag :: enable interrupts
}