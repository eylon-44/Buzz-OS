// sys_close // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Close an open file.

    close() closes a file descriptor, so that it no longer refers to
    any file and may be reused.

    close() returns zero on success, or -1 on error.

    int syscall(SYS_close, int fd);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_close(int_frame_t* param)
{
    param->eax = fs_close(param->ebx);
}