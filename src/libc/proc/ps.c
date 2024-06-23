// proc/ps.c // ~ eylon

#include <proc.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Get information about running processes.
    
    sys_ps() sets [psbuff] with a list of [count] running processes. The function
    always returns the number of running processes. [psbuff] should be large enough
    to contain atleast sizeof(struct ps)*[count] bytes.

    Call with [count] set to 0 to get the number of running processes, allocate a buffer
    of sizeof(struct ps)*[count] bytes, and call again, now with [count] set to the real
    number of running processes.

    Note that new processes may be created or destroyed between subsequent calls, so that
    the return value from the first call may differ from the second one.

    #include <proc.h>
    int ps(struct ps* psbuff, int count);
*/
int ps(struct ps* psbuff, int count)
{
    return syscall(SYS_ps, psbuff, count);
}