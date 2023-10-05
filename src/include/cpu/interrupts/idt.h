// Interrupt Descriptor Table Header // ~ eylon

#if !defined(IDT_H)
#define IDT_H

#include <utils/type.h>

typedef struct
{
    u16_t offset_low;       // lower 16 bits of handler function address
    u16_t segment_selector; // segment selector, offset in the GDT
    u8_t  reserved0;        // reserved byte is 0
    u8_t  flags;            // Flags:
                            //  bit 0-4: 
                            //  bit 5-6: DPL, required privilege level to execute, high-[0 (kernel), 1, 2, 3 (user)]-low 
                            //  bit   7: Present, [1]=interrupt is present, valid, and should be used
    u16_t offset_high;      // higher 16 bits of handler function address

} __attribute__((packed)) IDTEntry;

#endif