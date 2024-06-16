// stdio/printf.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

/* The classic printf.

    On sucess, returns the number of characters that got printed (excluding the null terminator).
    On failure, returns a negative number.

    #include <stdio.h>
    int printf(const char* format, ...);
*/
int printf(UNUSED const char* format, ...)
{
    // https://stackoverflow.com/a/23842944/22586830
    // syscall for allocating pages (sbrk/brk/?) -> malloc -> asprintf -> printf
    return 0;
}