// Syscall Handler Header File // ~ eylon

#if !defined(SYSCALL_H)
#define SYSCALL_H

#include <cpu/interrupts/isr.h>

// Number of syscalls
#define SYSCALL_NUM 256

typedef void (*syscall_t)(int_data_t*);

void init_syscall();

#endif