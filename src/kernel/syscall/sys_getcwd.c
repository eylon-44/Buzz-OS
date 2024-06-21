// sys_getcwd // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <kernel/process/pm.h>
#include <libc/stddef.h>

/* Get current working directory.

    The getcwd() syscall returns a null-terminated string containing an
    absolute pathname that is the current working directory of the
    calling process. The pathname is stored in [buff], which is [size] bytes
    long buffer.

    If the length of the absolute pathname of the current working
    directory, including the terminating null byte, exceeds [size]
    bytes, NULL is returned.
    
    On success, a pointer to [buff] is returned. On error, NULL is returned.

    char* syscall(SYS_getcwd, char* buff, size_t size);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_getcwd(int_frame_t* param)
{
    param->eax = (size_t) fs_build_path(pm_get_active()->cwd, (char*) param->ebx, param->ecx);
}
