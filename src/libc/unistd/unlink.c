// unistd/unlink.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Delete a file.

    unlink() deletes a file from the filesystem.

    On success, 0 is returned and on error -1.

    #include <unistd.h>
    int unlink(const char* pathname);
*/
int unlink(const char* pathname)
{
    return syscall(SYS_unlink, pathname);
}