// sys/stat/stat.c // ~ eylon

#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Get file status.

    stat() and fstat() retrieve information about the file pointed
    to by [pathname] or refrenced by [fd] into the buffer [statbuff]
    of the type struct stat, which is defined at sys/stat.h.

    On success, zero is returned.  On error, -1 is returned.
    
    #include <sys/stat.h>
    int stat(const char* pathname, struct stat* statbuf);
    int fstat(int fd, struct stat* statbuf);
*/
int stat(const char* pathname, struct stat* statbuf)
{
    return syscall(SYS_stat, pathname, statbuf);
}
int fstat(int fd, struct stat* statbuf)
{
    return syscall(SYS_fstat, fd, statbuf);
}