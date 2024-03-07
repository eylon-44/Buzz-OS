// ctype/isalpha.c // ~ eylon

#include <ctype.h>

/*  Check if a character is an alphabetic character.

    Check if a character is in the ascii range of 'a'-'z' or 'A'-'Z'.

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isalpha(int c);
*/
int isalpha(int c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}