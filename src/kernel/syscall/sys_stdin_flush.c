// sys_stdin_flush // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>
#include <kernel/ui.h>
#include <libc/stddef.h>

/* Clear the caller's stdin buffer.

    void syscall(SYS_stdin_flush);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_stdin_flush(UNUSED int_frame_t* param)
{
    ui_stdin_flush(pm_get_active()->tab);
}