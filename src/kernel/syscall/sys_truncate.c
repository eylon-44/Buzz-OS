// sys_truncate // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Truncate a file to a specified length.

    The sys_truncate() and sys_ftruncate() syscalls cause the regular file named by [path]
    or referenced by [fd] to be truncated to a size of precisely [length] bytes.

    If the file previously was larger than this size, the extra data is lost. If the
    file previously was shorter, it is extended, and the extended part reads as null bytes.

    The file offset is not changed.

    With ftruncate(), the file must be open for writing; with truncate(), the file must be writable.

    On success, 0 is returned. On error, -1 is returned.

    int syscall(SYS_truncate, const char* path, size_t length);
    int syscall(SYS_ftruncate, int fd, size_t length);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_truncate(int_frame_t* param)
{
    param->eax = fs_truncate((const char*) param->ebx, param->ecx);
}
void sys_ftruncate(int_frame_t* param)
{
    param->eax = fs_ftruncate(param->ebx, param->ecx);
}