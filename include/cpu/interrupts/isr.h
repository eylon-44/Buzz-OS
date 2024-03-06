// Interrupt Service Routine/Interrupt Handler Header File // ~ eylon

#if !defined(ISR_H)
#define ISR_H

#include <libc/stdint.h> 

// Define IRQ locations in the IDT
#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

// Using uint32_t values as the stack values are 4 bytes long (32 bit mode)
// Interrupt request stack data
typedef struct 
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;       // cpu registers values from interrupted code
    uint32_t interrupt_number, error_code;                 // interrupt number and error code
    uint32_t eip, cs, eflags;                              // irq information pushed by the cpu
} __attribute__((packed)) InterruptData;

typedef void (*isr_t)(); 

void init_interrupt();
void set_interrupt_handler(uint8_t index, isr_t func);


#endif