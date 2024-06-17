// unistd/sbrk.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Change data segment size.
    
    sbrk() changes the location of the program break, which defines the end of
    the process's data segment (the location of the _end symbol created by the
    linker). Increasing the program break has the effect of allocating memory to
    the process; decreasing the break deallocates memory.

    sbrk() increments the program's data space by [increment] bytes. Calling sbrk()
    with [increment] set to 0 can be used to find the current location of the program break.

    On success, sbrk() returns the new program break. On error, NULL is returned.

    #include <stdlib.h>
    void* sbrk(size_t increment);
*/
void* sbrk(size_t increment)
{
    return (void*) syscall(SYS_sbrk, increment);
}