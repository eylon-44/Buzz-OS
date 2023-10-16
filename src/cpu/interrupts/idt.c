// Interrupt Descriptor Table // ~ eylon

#include <cpu/interrupts/idt.h>
#include <utils/type.h>

static InterruptDescriptor idt[IDT_ENTRIES_COUNT];
static IDTR idtr;

// Set an interrupt descriptor in the IDT array
void set_interrupt_descriptor(u8_t index, u32_t handler_address)
{
    idt[index].offset_low       = LOW_16(handler_address);
    idt[index].segment_selector = (u16_t) 8; // [TODO] set a kernel segment
    idt[index].reserved_zero    = (u8_t) 0b00000000;
    idt[index].type_attributes  = (u8_t) 0b10001110;
    idt[index].offset_high      = HIGH_16(handler_address);
}

// Set the IDT descriptor and load the IDT
void load_idt()
{
    // set the IDT descriptor
    idtr.size   = (u16_t) sizeof(idt) - 1;
    idtr.offset = (u32_t) &idt;

    // load idtr
    __asm__ __volatile__("lidt (%0)" : : "r" (&idtr));
}