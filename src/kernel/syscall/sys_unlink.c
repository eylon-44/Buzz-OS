// sys_unlink // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Delete a file.

    unlink() deletes a file from the filesystem.

    On success, zero is returned and on error -1.

    int syscall(SYS_unlink, const char* pathname);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_unlink(int_frame_t* param)
{
    param->eax = fs_remove((const char*) param->ebx);
}