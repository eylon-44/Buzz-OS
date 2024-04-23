// Syscall Handler // ~ eylon

#include <kernel/syscall/syscall.h>
#include <cpu/interrupts/isr.h>
#include <libc/stddef.h>

// Array of syscall handlers arranged by their syscall number
static syscall_t syscall_handlers[SYSCALL_NUM];

// Common syscall handler; calls the required syscall handler
static void common_syscall_handler(int_data_t* param)
{
    // Check that the syscall number is valid; if not, exit
    if (param->eax >= SYSCALL_NUM || syscall_handlers[param->eax] == NULL) {
        param->eax = 111;   // invalid; [TODO] do protocol and stuff
        return;
    }

    // If is valid, call the handler
    syscall_handlers[param->eax](param);
}

// Initiate the syscall service
void init_syscall()
{
    // Set the syscall interrupt handler
    set_interrupt_handler(SYSCALL_INT, common_syscall_handler);

    // Initiate the syscall handlers array with NULL pointers
    for (int i = 0; i < SYSCALL_NUM; i++) {
        syscall_handlers[i] = NULL;
    }
}