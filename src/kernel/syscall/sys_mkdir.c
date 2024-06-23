// sys_mkdir // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <libc/fcntl.h>

/* Create a new directory.
    
    sys_mkdir() attempts to create a directory named [pathname].

    The mkdir() function shall behave as if it is implemented as
    follows:

    int mkdir(const char *path)
    {
        return open(path, O_WRONLY|O_CREAT|O_TRUNC|O_DIRECTORY);
    }

    int syscall(SYS_mkdir, const char* pathname);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_mkdir(int_frame_t* param)
{
    param->eax = fs_open((char*) param->ebx, O_WRONLY|O_CREAT|O_TRUNC|O_DIRECTORY);
}