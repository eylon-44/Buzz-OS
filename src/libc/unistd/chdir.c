// unistd/chdir.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Change working directory.
    
    The chdir() function shall cause the directory named by the
    pathname pointed to by the [path] argument to become the current
    working directory; that is, the starting point for path searches
    for pathnames not beginning with '/'.

    Upon successful completion, 0 shall be returned. Otherwise, -1 shall be
    returned and the current working directory shall remain unchanged.

    #include <unistd.h>
    int chdir(const char* path);
*/
int chdir(const char* path)
{
    return syscall(SYS_chdir, path);
}