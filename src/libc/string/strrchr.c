// string/strrchr.c // ~ eylon

#include <string.h>

/*  Locate the last given character in a string.

    The strrchr() function shalllocate the last occurrence of [c]
    in the string pointed to by [s]. The terminating NULL character
    is considered to be part of the string.
    
    Upon successful completion, strrchr() shall return a pointer to
    the byte, or a null pointer if [c] is not found in the string.

    #include <string.h>
    char* strrchr(const char* str, int c);
*/
char* strrchr(const char *str, int c)
{
    char* last = NULL;

    while (*str != '\0')
    {
        if (*str == (char) c) {
            last = (char*) str;
        }
        str++;
    }

    if ((char) c == '\0') {
        last = (char*) str;
    }

    return last;
}