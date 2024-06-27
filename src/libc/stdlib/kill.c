// stdlib/kill.c // ~ eylon

#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Terminate a process.

    On success, 0 is returned, and on error, -1.

    #include <stdlib.h>
    int kill(int pid);    
*/
int kill(int pid)
{
    return syscall(SYS_kill, pid);
}