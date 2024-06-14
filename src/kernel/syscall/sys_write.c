// sys_sched_yield // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Write to a file descriptor.
    
    sys_write() writes up to [count] bytes from the buffer starting at [buff]
    to the file referred to by the file descriptor [fd].

    ssize_t syscall(SYS_write, int fd, const void* buff, size_t count);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_write(int_frame_t* param)
{
    param->eax = fs_write(param->ebx, (const void*) param->ecx, param->edx);
}