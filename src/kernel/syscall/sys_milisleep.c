// sys_milisleep // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/scheduler.h>
#include <drivers/timer.h>

/* Sleep the calling process.
    
    The milisleep() system call causes the calling process to sleep until the
    number of real-time mili-seconds specified in [miliseconds] have elapsed.

    The function always returns 0.

    int syscall(SYS_milisleep, size_t miliseconds);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_milisleep(int_frame_t* param)
{
    param->eax = 0;
    sched_sleep(sched_get_active()->pid, (param->ebx * TIMER_TICK_HZ)/1000);
}