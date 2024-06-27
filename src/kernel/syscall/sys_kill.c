// sys_kill // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>

/* Terminate a process.

    On success, 0 is returned, and on error, -1.

    int syscall(SYS_kill, int pid);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_kill(int_frame_t* param)
{
    process_t* proc = pm_get_process_by_id(param->ebx);
    if (proc != NULL) {
        pm_kill(proc);
        param->eax = 0;
    }
    else {
        param->eax = -1;
    }
}