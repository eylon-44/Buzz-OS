// unistd/milisleep.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Sleep the calling process.
    
    The milisleep() system call causes the calling process to sleep until the
    number of real-time mili-seconds specified in [miliseconds] have elapsed.

    The function always returns 0.

    #include <unistd.h>
    int milisleep(size_t miliseconds);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
int milisleep(size_t miliseconds)
{
    return syscall(SYS_milisleep, miliseconds);
}