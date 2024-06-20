// sys_lseek // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Offset an open file.
    
    sys_lseek() repositions the file offset of the open file description
    associated with the file descriptor [fd] to the argument [offset]
    according to the directive [whence].
    
    Upon successful completion, sys_lseek() returns the resulting offset location
    as measured in bytes from the beginning of the file. On error, -1 is returned.

    int syscall(SYS_lseek, int fd, int offset, int whence);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_lseek(int_frame_t* param)
{
    param->eax = fs_lseek(param->ebx, param->ecx, param->edx);
}