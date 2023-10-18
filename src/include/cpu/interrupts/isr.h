// Interrupt Service Routine/Interrupt Handler Header File // ~ eylon

#if !defined(ISR_H)
#define ISR_H

#include <utils/type.h> 

// Define IRQ locations in the IDT
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

// Using u32_t values as the stack values are 4 bytes long (32 bit mode)
// CPU state struct
typedef struct
{
    u32_t edi;
    u32_t esi;
    u32_t ebp;
    u32_t esp;
    u32_t ebx;
    u32_t edx;
    u32_t ecx;
    u32_t eax;
} __attribute__((packed)) CPUState;

// Interrupt Request stack state struct
typedef struct
{
    u32_t error_code;
    u32_t eip;
    u32_t cs;
    u32_t eflags;
} __attribute__((packed)) IRQStackState;

typedef void (*isr_t)(); 

void init_interrupt();
void set_interrupt_handler(u8_t index, isr_t func);


#endif