// string/strcpy.c // ~ eylon

#include <string.h>
#include <stddef.h>

/*  Copy a string.
       
    The  strcpy() function copies the string pointed to by [src], including the terminating 
    null byte ('\0'), to the buffer pointed to by [dest]. The strings may not overlap, and 
    the destination string [dest] must be large enough to receive the copy. Beware of buffer overruns!

    The strcpy() function returns a pointer to the destination string [dest].

    #include <string.h>
    char* strcpy(char* dest, const char* src);
*/
char* strcpy(char* dest, const char* src)
{
    size_t i;

    for (i = 0; src[i] != 0; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = src[i];

    return dest;
}