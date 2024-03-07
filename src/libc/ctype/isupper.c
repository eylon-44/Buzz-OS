// ctype/isupper.c // ~ eylon

#include <ctype.h>

/*  Check if a character is uppercased.

    Check if a character ranging from 'a'-'z' or from 'A'-'Z' is uppercased or not.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.
    Returns zero if [c] is not an alphabetic character.

    #include <ctype.h>
    int isupper(int c);
*/
int isupper(int c)
{
    if (!isalpha(c)) return 0;
    return !(c & (1 << 5));
}