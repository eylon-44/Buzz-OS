// stdio/asprintf.c // ~ eylon

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

/* https://linux.die.net/man/3/asprintf */
int asprintf(char **strp, const char *fmt, ...)
{
    size_t len;
    va_list args;
    
    va_start(args, fmt);
    len = vasprintf(strp, fmt, args);

    va_end(args);
    return len;
}