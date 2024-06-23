// sys/stat/mkdir.c // ~ eylon

#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Create a new directory.
    
    mkdir() attempts to create a directory named [pathname].

    The mkdir() function shall behave as if it is implemented as
    follows:

    int mkdir(const char *path)
    {
        return open(path, O_WRONLY|O_CREAT|O_TRUNC|O_DIRECTORY);
    }

    #include <sys/stat.h>
    int mkdir(const char* pathname);
*/
int mkdir(const char* pathname)
{
    return syscall(SYS_mkdir,  pathname);
}