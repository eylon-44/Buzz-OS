// Interrupt Service Routine :: Interrupt Handler // ~ eylon

#include <cpu/interrupts/isr.h>
#include <cpu/interrupts/pic.h>
#include <cpu/interrupts/idt.h>
#include <utils/type.h>

#define INTERRUPT_HANDLER(index) extern void (interrupt_handler_##index)(); \
                                 set_interrupt_descriptor(index, (u32_t) interrupt_handler_##index);

isr_t interrupt_handlers[IDT_ENTRIES_COUNT];


void set_interrupt_handler(u8_t index, isr_t func)
{
    interrupt_handlers[index] = func;
    // unmask if it's an IRQ
    if (index >= IRQ0 && index <= IRQ15) unmask_irq(index);
}

#include <drivers/screen.h> // [DEBUG]
void interrupt_handler(const InterruptData int_data)
{
    static int loc = 0;
    //interrupt_handlers[interrupt]();
    for (int i = 0; i < int_data.interrupt_number; i++) {
        kprint_at("I", int_data.interrupt_number, i*2+loc);
    }
    if (int_data.interrupt_number > 31) {
        kprint("EOI", VGA_BG_GREEN);
        pic_eoi(int_data.interrupt_number-32);
    }
    loc+=80;
    return;
}

void init_interrupt()
{
    __asm__ volatile ("cli");       // clear interrupt flag :: disable interrupts

    init_pic();                     // config and initiate the PIC

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

    __asm__ volatile ("sti");       // set interrupt flag   :: enable interrupts

    mask_all_irq();
}