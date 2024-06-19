// sys_creat // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <libc/fcntl.h>

/* Create and open a file.

    The creat() function shall behave as if it is implemented as
    follows:

    int creat(const char *path)
    {
        return open(path, O_WRONLY|O_CREAT|O_TRUNC);
    }

    int syscall(SYS_creat, const char* path);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_creat(int_frame_t* param)
{
    param->eax = fs_open((char*) param->ebx, O_WRONLY|O_CREAT|O_TRUNC);
}