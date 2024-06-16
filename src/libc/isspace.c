// ctype/isalnum.c // ~ eylon

#include <ctype.h>

/* Test for a blank character.

    Returns 1 if the [c] is a blank character, otherwise 0.

    #include <ctype.h>
    int isspace(int c);
*/
int isspace(int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}