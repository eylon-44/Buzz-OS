// ctype/isalnum.c // ~ eylon

#include <ctype.h>

/*  Check for an alphanumeric character.

    Check for an alphanumeric character; this is equivalent to (isalpha(c) || isdigit(c)).

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isalnum(int c);
*/
int isalnum(int c)
{
    return (isalpha(c) || isdigit(c));
}