// sys_sched_yield // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/scheduler.h>
#include <libc/stddef.h>

/* Give up execution time.
    sched_yield() causes the calling process to relinquish the CPU.

    void syscall(SYS_sched_yield);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_sched_yield(UNUSED int_frame_t* param)
{
    sched_switch_next();
}