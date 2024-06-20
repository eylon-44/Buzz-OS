// sys_militime // ~ eylon

#include <kernel/syscall.h>
#include <drivers/timer.h>
#include <libc/stddef.h>

/* Get active time.
    
    The militime() system call returns the number of miliseconds passed since the system startup.

    size_t syscall(SYS_militime);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_militime(int_frame_t* param)
{
    extern size_t ticks;
    param->eax = (ticks * 1000) / TIMER_TICK_HZ;
}