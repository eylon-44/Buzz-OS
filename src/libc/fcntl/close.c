// fcntl/close.c // ~ eylon

#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Close an open file.

    close() closes a file descriptor, so that it no longer refers to
    any file and may be reused.

    close() returns zero on success, or -1 on error.

    #include <fcntl.h>
    int close(int fd);
*/
int close(int fd)
{
    return syscall(SYS_close, fd);
}

