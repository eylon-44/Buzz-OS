// stdlib/atoi.c // ~ eylon

#include <stdlib.h>
#include <ctype.h>

/* Convert a null terminated string to an integer.

    The atoi() function converts the initial portion of the string
    pointed to by nptr to int.

    Returns the converted value on success, or 0 on failure.

    #include <stdlib.h>
    int atoi(const char *nptr);
*/
int atoi(const char *nptr)
{
    int num = 0;
    int sign = 1;

    // Skip leading whitespace
    while (isspace(*nptr)) {
        nptr++;
    }

    // Check for sign
    if (*nptr == '-' || *nptr == '+') {
        if (*nptr == '-') {
            sign = -1;
        }
        nptr++;
    }

    // Convert characters to integer
    while (isdigit(*nptr)) {
        num = num * 10 + *nptr - '0';
        nptr++;
    }

    return num * sign;
}