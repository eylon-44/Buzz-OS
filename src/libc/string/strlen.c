// string/strlen.c // ~ eylon

#include <string.h>
#include <stddef.h>

/*  Calculate the length of a string.

    The strlen() function calculates the length of the string pointed to by [s], 
    excluding the terminating null byte ('\0').

    The strlen() function returns the number of bytes in the string pointed to by [s].

    #include <string.h>
    size_t strlen(const char* s);
*/
size_t strlen(const char* s)
{
    size_t len;

    for (len = 0; s[len] != 0; len++)
        ;

    return len;
}