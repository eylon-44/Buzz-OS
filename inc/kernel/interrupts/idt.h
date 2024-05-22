// Interrupt Descriptor Table Header File // ~ eylon

#if !defined(IDT_H)
#define IDT_H

#include <libc/stdint.h>

#define IDT_ENTRIES_COUNT 256

// Type gates
typedef enum
{
    GT_32BIT_INT  = 0b1110,
    GT_32BIT_TRAP = 0b1111
} gate_type_t;

// Interrupt descriptor
typedef struct
{
    uint16_t offset_low;       // lower 16 bits of the starting address of the interrupt handler code.
    uint16_t segment_selector; // segment selector in which the segment handler code is in
    uint8_t  reserved_zero;    // reserved null byte :: 0b00000000
    struct
    {
        gate_type_t gate_type : 4;  // [0-3] gate type; type of gate this interrupt descriptor represents
        uint8_t     _reserved : 1;  // [4]   reserved; set to 0 at initiation
        uint8_t     dpl       : 2;  // [5-6] DPL; privilige levels which are allowed to access this interrupt via the INT instruction
        uint8_t     present   : 1;  // [7]   present bit; must be set to [1] for the descriptor to be valid

    } __attribute__((packed)) type_attributes;
    
    uint16_t offset_high;      // higher 16 bits of the starting address of the interrupt handler code.bnn 

} __attribute__((packed)) InterruptDescriptor;

// Interrupt descriptor table register :: IDT descriptor, 48 bits
typedef struct
{
    uint16_t size;     // one less than the size of the IDT in bytes
    uint32_t offset;   // starting address of the IDT
} __attribute__((packed)) IDTR;


void set_interrupt_descriptor(uint8_t index, uint32_t handler_address, gate_type_t gate_type, uint8_t dpl);
void load_idt();

#endif