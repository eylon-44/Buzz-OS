// Interrupt Descriptor Table Header File // ~ eylon

#if !defined(IDT_H)
#define IDT_H

#include <utils/type.h>

#define IDT_ENTRIES_COUNT 48

// Interrupt descriptor: 64 bits
typedef struct
{
    u16_t offset_low;       // lower 16 bits of the starting address of the interrupt handler code.
    u16_t segment_selector; // segment selector :: offset in the GDT :: which segment is the handler code in
    u8_t  reserved_zero;    // reserved null byte :: 0b00000000
    u8_t  type_attributes;  // type attributes :: full explanation can be found in "documentation/INERRUPTS.md"
                            //  | bits |    name   | description
                            //  |  0-3 | Gate Type | a 4-bit value which defines the type of gate this interrupt descriptor represents
                            //  |    4 |     0     | reserved
                            //  |  5-6 |    DPL    | a 2-bit value to define cpu privilige levels which are allowed to access this interrupt via the INT instruction.
                            //  |    7 |     P     | present bit, must be set to [1] for the descriptor to be valid.
    u16_t offset_high;      // higher 16 bits of the starting address of the interrupt handler code.bnn 

} __attribute__((packed)) InterruptDescriptor;

// Interrupt descriptor table register :: IDT descriptor, 48 bits
typedef struct
{
    u16_t size;     // one less than the size of the IDT in bytes
    u32_t offset;   // starting address of the IDT
} __attribute__((packed)) IDTR;


void set_interrupt_descriptor(u8_t index, u32_t handler_address);
void load_idt();

#endif