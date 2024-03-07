// ctype/toupper.c // ~ eylon

#include <ctype.h>

/*  Convert a character from lowercase to uppercase

    toupper() converts lowercase letters to uppercase ones.
    
    The value returned is that of the converted letter.

    #include <ctype.h>
    int toupper(int c);
*/
int toupper(int c)
{
    return c & ~(1 << 5);
}