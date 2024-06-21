// unistd/getcwd.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>
#include <stddef.h>

/* Get current working directory.

    The getcwd() syscall returns a null-terminated string containing an
    absolute pathname that is the current working directory of the
    calling process. The pathname is stored in [buff], which is [size] bytes
    long buffer.

    If the length of the absolute pathname of the current working
    directory, including the terminating null byte, exceeds [size]
    bytes, NULL is returned.
    
    On success, a pointer to [buff] is returned. On error, NULL is returned.

    #include <unistd.h>
    char* getcwd(char* buff, size_t size);
*/
char* getcwd(char* buff, size_t size)
{
    return (char*) syscall(SYS_getcwd, buff, size);
}