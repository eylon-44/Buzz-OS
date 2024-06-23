// sys_reanme // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Change the name or location of a file.
    
    The rename() syscall renames a file and moves it between directories if required.
    
    On success, 0 is returned.  On error, -1 is returned.

    int syscall(SYS_rename, const char* oldpath, const char* newpath);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_rename(int_frame_t* param)
{
    param->eax = fs_rename((char*) param->ebx, (char*) param->ecx);
}