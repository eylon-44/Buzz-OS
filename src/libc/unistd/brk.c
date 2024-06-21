// unistd/brk.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Change data segment size.
    
    brk() changes the location of the program break, which defines the end of
    the process's data segment (the location of the _end symbol created by the
    linker). Increasing the program break has the effect of allocating memory to
    the process; decreasing the break deallocates memory.

    brk() sets the end of the data segment to the value specified by [addr], when
    that value is reasonable, the system has enough memory, and the process does
    not exceed its maximum data size.

    On success, brk() returns zero. On error, -1 is returned.

    #include <unistd.h>
    int brk(void *addr);
*/
int brk(void *addr)
{
    return syscall(SYS_brk, addr);
}