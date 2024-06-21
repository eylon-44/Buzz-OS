// sys_stat // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Get file status.

    stat() and fstat() retrieve information about the file pointed
    to by [pathname] or refrenced by [fd] into the buffer [statbuff]
    of the type struct stat, which is defined at sys/stat.h.

    On success, zero is returned.  On error, -1 is returned.

    int syscall(SYS_stat, const char* pathname, struct stat* statbuf);
    int syscall(SYS_fstat, int fd, struct stat* statbuf);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_stat(int_frame_t* param)
{
    param->eax = fs_stat((char*) param->ebx, (struct stat*) param->ecx);
}
void sys_fstat(int_frame_t* param)
{
    param->eax = fs_fstat(param->ebx, (struct stat*) param->ecx);
}