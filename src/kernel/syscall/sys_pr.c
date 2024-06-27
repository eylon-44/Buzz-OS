// sys_pr // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/scheduler.h>
#include <stddef.h>

/* Change the priority of a process.
    
    Set the priority of the process with process ID [pid] to [priority].
    [priority] must have a value between PRIORITY_MIN and PRIORITY_MAX defined
    in <limits.h>

    The function returns the new priority of process [pid], or -1 if an error occured.

    int syscall(SYS_pr, int pid, int priority);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_pr(int_frame_t* param)
{
    process_t* proc = pm_get_process_by_id(param->ebx);
    if (proc == NULL) {
        param->eax = -1;
        return;
    }
    param->eax = sched_set_priority(proc, param->ecx);
}