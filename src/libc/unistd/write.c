// unistd/write.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Write to a file descriptor.
    
    sys_write() writes up to [count] bytes from the buffer starting at [buff]
    to the file referred to by the file descriptor [fd].

    On success, the number of bytes written is returned. On failure, -1 is returned.

    #include <unistd.h>
    ssize_t write(int fd, const void* buff, size_t count);
*/
ssize_t write(int fd, const void* buff, size_t count)
{
    return syscall(SYS_write, fd, buff, count);
}