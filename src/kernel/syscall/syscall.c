// Syscall Handler // ~ eylon

#include <kernel/syscall.h>
#include <kernel/interrupts/isr.h>
#include <libc/stddef.h>
#include <libc/sys/syscall.h>

/* Macro to extern and set a syscall handler in the handlers list.
    Take [name] out of sys_[name] or SYS_[name]. */
#define SYSCALL_HANDLER(name) \
    extern void (sys_##name)(int_frame_t* param); \
    syscall_handlers[SYS_##name] = sys_##name

// Array of syscall handlers arranged by their syscall number
static syscall_t syscall_handlers[SYSCALL_NUM];

// Common syscall handler; calls the required syscall handler
static void common_syscall_handler(int_frame_t* param)
{
    // Check that the syscall number is valid; if not, abort
    if (param->eax >= SYSCALL_NUM || syscall_handlers[param->eax] == NULL) {
        param->eax = -1;   // invalid; [TODO] do protocol and stuff
        return;
    }

    // If syscall number is valid, call the handler
    syscall_handlers[param->eax](param);
}

// Initiate the syscall service
void init_syscall()
{
    // Set the syscall interrupt handler
    set_interrupt_handler(SYS_int, common_syscall_handler);

    // Initiate the syscall handlers list NULL
    for (int i = 0; i < SYSCALL_NUM; i++) {
        syscall_handlers[i] = NULL;
    }

    // Set the list with handlers
    SYSCALL_HANDLER(exit);
    SYSCALL_HANDLER(read);
    SYSCALL_HANDLER(write);
    SYSCALL_HANDLER(open);
    SYSCALL_HANDLER(close);
    SYSCALL_HANDLER(execve);
    SYSCALL_HANDLER(creat);
    SYSCALL_HANDLER(brk);
    SYSCALL_HANDLER(sbrk);
    SYSCALL_HANDLER(sched_yield);
    SYSCALL_HANDLER(unlink);
    SYSCALL_HANDLER(milisleep);
    SYSCALL_HANDLER(lseek);
    SYSCALL_HANDLER(mkdir);
    SYSCALL_HANDLER(truncate);
    SYSCALL_HANDLER(ftruncate);
    SYSCALL_HANDLER(militime);
    SYSCALL_HANDLER(stat);
    SYSCALL_HANDLER(fstat);
    SYSCALL_HANDLER(rmdir);
    SYSCALL_HANDLER(getcwd);
    SYSCALL_HANDLER(chdir);
    SYSCALL_HANDLER(rename);
}