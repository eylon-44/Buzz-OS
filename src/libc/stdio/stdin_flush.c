// stdio/stdin_flush.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Clear the caller's stdin buffer.

    #include <stdio.h>
    void stdin_flush();
*/
void stdin_flush()
{
    syscall(SYS_stdin_flush);
}