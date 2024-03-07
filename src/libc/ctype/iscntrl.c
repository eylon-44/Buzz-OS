// ctype/iscntrl.c // ~ eylon

#include <ctype.h>

/*  Check for a control character.

    Check for a control character; those include character codes 0 through 31 and 127.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int iscntrl(int c);
*/
int iscntrl(int c)
{
    return (c >= 0 && c <= 31) | (c == 127);
}