// sys_sched_yield // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/scheduler.h>
#include <libc/stddef.h>

/* Give up execution time.
    sched_yield() causes the calling process to relinquish the CPU.

    void syscall(SYS_sched_yield);
*/
void sys_sched_yield(UNUSED int_frame_t* param)
{
    sched_switch_next();
}