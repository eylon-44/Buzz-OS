// time/militime.c // ~ eylon

#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Get active time.
    
    The militime() function returns the number of miliseconds passed since the system startup.

    #include <time.h>
    size_t militime();
*/
size_t militime()
{
    return syscall(SYS_militime);
}