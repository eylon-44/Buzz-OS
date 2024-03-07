// ctype/isxdigit.c // ~ eylon

#include <ctype.h>

/*  Check if a character is a hexadecimal digit.

    Checks for hexadecimal digit, that is, one of 0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isxdigit(int c);
*/
int isxdigit(int c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}