// string/strcpy.c // ~ eylon

#include <string.h>
#include <stddef.h>

/*  Copy a string.
       
    The  strncpy() function copies the string pointed to by [src], including the terminating 
    null byte ('\0'), to the buffer pointed to by [dest]. The strings may not overlap, and 
    the destination string [dest] must be large enough to receive the copy. Beware of buffer overruns!
    The function copies no more than [n] bytes of [src] into [dest]. Warning: If there is no null 
    byte among the first [n] bytes of [src], the string placed in [dest] will not be null-terminated. 
    If the length of [src] is less than [n], strncpy() writes additional null bytes to dest to ensure 
    that a total of [n] bytes are written.

    The strncpy() function returns a pointer to the destination string [dest].

    #include <string.h>
    char* strncpy(char* dest, const char* src, size_t n);
*/
char* strncpy(char* dest, const char* src, size_t n)
{
    size_t i;

    for (i = 0; i < n && src[i] != 0; i++)
        dest[i] = src[i];

    for (; i < n; i++)
        dest[i] = 0;

    return dest;
}