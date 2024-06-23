// stdio/rename.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Change the name or location of a file.
    
    The rename() function renames a file and moves it between directories if required.
    
    On success, 0 is returned.  On error, -1 is returned.

    #include <stdio.h>
    int rename(const char* oldpath, const char* newpath);
*/
int rename(const char* oldpath, const char* newpath)
{
    return syscall(SYS_rename, oldpath, newpath);
}