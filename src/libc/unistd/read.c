// unistd/read.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Read from a file descriptor.

    read() attempts to read up to [count] bytes from file descriptor fd
    into the buffer starting at [buff].

    On files that support seeking, the read operation commences at
    the file offset, and the file offset is incremented by the number
    of bytes read. If the file offset is at or past the end of file,
    no bytes are read, and read() returns zero.

    #include <unistd.h>
    ssize_t read(int fd, void* buff, size_t count);
*/
ssize_t read(int fd, void* buff, size_t count)
{
    return syscall(SYS_read, fd, buff, count);
}