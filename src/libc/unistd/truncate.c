// unistd/truncate.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Truncate a file to a specified length.

    The truncate() and ftruncate() functions cause the regular file named by [path]
    or referenced by [fd] to be truncated to a size of precisely [length] bytes.

    If the file previously was larger than this size, the extra data is lost. If the
    file previously was shorter, it is extended, and the extended part reads as null bytes.

    The file offset is not changed.

    With ftruncate(), the file must be open for writing; with truncate(), the file must be writable.

    On success, 0 is returned. On error, -1 is returned.

    #include <unistd.h>
    int truncate(const char *path, size_t length);
    int ftruncate(int fd, size_t length);
*/
int truncate(const char *path, size_t length)
{
    return syscall(SYS_truncate, path, length);
}
int ftruncate(int fd, size_t length)
{
    return syscall(SYS_ftruncate, fd, length);
}