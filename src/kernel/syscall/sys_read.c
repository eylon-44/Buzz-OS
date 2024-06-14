// sys_read // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <libc/stddef.h>

/* 
    ssize_t syscall(SYS_read, int fd, void* buff, size_t count);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_read(int_frame_t* param)
{
    param->eax = fs_read(param->ebx, (void*) param->ecx, param->edx);
}