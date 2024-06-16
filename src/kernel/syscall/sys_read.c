// sys_read // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <libc/stddef.h>

/* Read from a file descriptor.

    read() attempts to read up to [count] bytes from file descriptor fd
    into the buffer starting at [buff].

    On files that support seeking, the read operation commences at
    the file offset, and the file offset is incremented by the number
    of bytes read. If the file offset is at or past the end of file,
    no bytes are read, and read() returns zero.

    ssize_t syscall(SYS_read, int fd, void* buff, size_t count);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_read(int_frame_t* param)
{
    param->eax = fs_read(param->ebx, (void*) param->ecx, param->edx);
}