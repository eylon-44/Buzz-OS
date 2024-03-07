// ctype/isprint.c // ~ eylon

#include <ctype.h>

/*  Check for a graphical printable character.

    Check for any printable character including space; this is equivalent to (isgraph(c) || c == ' ').

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isprint(int c);
*/
int isprint(int c)
{
    return isgraph(c) || c == ' ';
}