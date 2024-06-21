// unistd/rmdir.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Delete a directory.

    rmdir() deletes a directory and all files in it from the filesystem.

    On success, zero is returned and on error -1.

    #include <unistd.h>
    int rmdir(const char* path);
*/
int rmdir(const char* path)
{
    return syscall(SYS_rmdir, path);
}