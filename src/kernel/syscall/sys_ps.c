// sys_ps // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>
#include <libc/proc.h>

/* Get information about running processes.
    
    sys_ps() sets [psbuff] with a list of [count] running processes. The function
    always returns the number of running processes. [psbuff] should be large enough
    to contain atleast sizeof(struct ps)*[count] bytes.

    Call with [count] set to 0 to get the number of running processes, allocate a buffer
    of sizeof(struct ps)*[count] bytes, and call again, now with [count] set to the real
    number of running processes.

    Note that new processes may be created or destroyed between subsequent calls, so that
    the return value from the first call may differ from the second one.

    int syscall(SYS_ps, struct ps* psbuff, int count);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_ps(int_frame_t* param)
{
    param->eax = pm_ps((struct ps*) param->ebx, param->ecx);
}