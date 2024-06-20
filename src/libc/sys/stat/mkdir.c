// sys/stat/mkdir.c // ~ eylon

#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Create a new directory.
    
    sys_mkdir() attempts to create a directory named [pathname].

    mkdir() returns 0 on success, and -1 on error.

    #include <sys/stat.h>
    int mkdir(const char* pathname);
*/
int mkdir(const char* pathname)
{
    return syscall(SYS_mkdir,  pathname);
}