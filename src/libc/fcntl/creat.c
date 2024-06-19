// fcntl/creat.c // ~ eylon

#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Create and open a file.

    The creat() function shall behave as if it is implemented as
    follows:

    int creat(const char *path)
    {
        return open(path, O_WRONLY|O_CREAT|O_TRUNC);
    }

    #include <fcntl.h>
    int creat(const char *path);
*/
int creat(const char *path)
{
    return syscall(SYS_creat, path);
}