// fcntl/open.c // ~ eylon

#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Open and possibly create a file.
    
    The open() system call opens the file specified by pathname and returns
    a file descriptor for that file. If the specified file does not exist, it
    may optionally (if O_CREAT is specified in flags) be created by open().

    The return value of open() is a file descriptor - a nonnegative integer
    that is an index to an entry in the process's table of open file descriptors. 
    The file descriptor is used in subsequent system calls (read, write, lseek, and
    more) to refer to the open file.

    The argument [flags] must include one of the following access modes: O_RDONLY,
    O_WRONLY, or O_RDWR. These request opening the file read-only, write-only, or
    read/write, respectively.

    In addition, zero or more file creation flags and file status flags can be bitwise
    ORed in flags.

    #include <fcntl.h>
    int open(const char *pathname, int flags);
*/
int open(const char *pathname, int flags)
{
    return syscall(SYS_open, pathname, flags);
}