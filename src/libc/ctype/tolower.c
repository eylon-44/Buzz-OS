// ctype/tolower.c // ~ eylon

#include <ctype.h>

/*  Convert a character from uppercase to lowercase

    tolower() converts uppercase letters to lowercase ones.
    
    The value returned is that of the converted letter.

    #include <ctype.h>
    int tolower(int c);
*/
int tolower(int c)
{
    return c | (1 << 5);
}