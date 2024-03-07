// ctype/isdigit.c // ~ eylon

#include <ctype.h>

/*  Check for a digit character.

    Check for a digit character, that is, characters in the range of '0'-'9'.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isdigit(int c);
*/
int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}