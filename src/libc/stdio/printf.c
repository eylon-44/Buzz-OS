// stdio/printf.c // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* The classic printf.

    On sucess, returns the number of characters that got printed (excluding the null terminator).
    On failure, returns a negative number.

    #include <stdio.h>
    int printf(const char* format, ...);
*/
int printf(const char* format, ...)
{
    char* str;
    size_t len;
    va_list args;
    
    va_start(args, format);
    len = vasprintf(&str, format, args);

    syscall(SYS_write, stdout, str, len);

    va_end(args);
    free(str);

    return len;
}