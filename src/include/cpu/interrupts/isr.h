// Interrupt Service Routine/Interrupt Handler Header File // ~ eylon

#if !defined(ISR_H)
#define ISR_H

#include <utils/type.h> 

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

void init_interrupt();

#endif