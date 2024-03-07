// ctype/isspace.c // ~ eylon

#include <ctype.h>

/*  Check for a white-space character.

    Check for a white-space character, those include: space, form-feed ('\f'), newline ('\n'), 
    carriage return ('\r'), horizontal tab ('\t'), and vertical tab ('\v').

    Returns nonzero if the character [c] falls into the tested case, and zero if not.

    #include <ctype.h>
    int isspace(int c);
*/
int isspace(int c)
{
    return (c >= 9 && c <= 13) || (c == ' ');
}