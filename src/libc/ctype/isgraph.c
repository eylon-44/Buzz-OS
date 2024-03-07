// ctype/isgraph.c // ~ eylon

#include <ctype.h>

/*  Check for a graphical printable character.

    Check for any printable character except space; those include character codes 33 through 126.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isgraph(int c);
*/
int isgraph(int c)
{
    return (c >= 33 && c <= 126);
}