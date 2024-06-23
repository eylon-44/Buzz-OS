// proc/priority.c // ~ eylon

#include <proc.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Change the priority of a process.
    
    Set the priority of the process with process ID [pid] to [priority].
    [priority] must have a value between PRIORITY_MIN and PRIORITY_MAX defined
    in <limits.h>

    The function returns the new priority of process [pid], or -1 if an error occured.

    int priority(int pid, int priority);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
int pr(int pid, int priority)
{
    return syscall(SYS_pr, pid, priority);
}