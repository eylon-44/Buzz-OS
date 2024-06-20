// sys_mkdir // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Create a new directory.
    
    sys_mkdir() attempts to create a directory named [pathname].

    mkdir() returns 0 on success, and -1 on error.

    int syscall(SYS_mkdir, const char* pathname);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_mkdir(int_frame_t* param)
{
    param->eax = fs_create((const char*) param->ebx, FS_NT_DIR);
}