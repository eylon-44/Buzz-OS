// sys_exit // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>

/* Terminate the calling process.
    [no-return] syscall(SYS_exit, int status);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_exit(UNUSED int_frame_t* param)
{
    pm_kill(pm_get_active());
}