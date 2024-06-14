// sys_exit // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/scheduler.h>
#include <libc/stddef.h>

/* Terminate the calling process.
    [no-return] syscall(SYS_exit, int status);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_exit(UNUSED int_frame_t* param)
{
    // Set the process to DONE and switch to the next process
    sched_set_status(sched_get_active(), PSTATUS_DONE);
    sched_switch_next();

    // [TODO] return status to waiter
}