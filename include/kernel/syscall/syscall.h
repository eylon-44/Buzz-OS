// Syscall Handler Header File // ~ eylon

#if !defined(SYSCALL_H)
#define SYSCALL_H

#include <cpu/interrupts/isr.h>

// Interrupt number to invoke a syscall
#define SYSCALL_INT 92  // 0x5C (SysCall)

// Number of syscalls
#define SYSCALL_NUM 256

typedef void (*syscall_t)(int_data_t*);

void init_syscall();

#endif