// string/strncmp.c // ~ eylon

#include <string.h>
#include <stdint.h>
#include <stddef.h>

/*  Compare two strings.

    The strncmp() function compares the two strings [s1] and [s2]. The comparison is 
    done using unsigned characters. The function compares only the first (at most) 
    [n] bytes of [s1] and [s2].

    strncmp() returns an integer indicating the result of the comparison, as follows:
        • 0, if the [s1] and [s2] are equal;
        • a negative value if [s1] is less than [s2];
        • a positive value if [s1] is greater than [s2].
    For a nonzero return value, the sign is determined by the sign of the difference between 
    the first pair of bytes (interpreted as unsigned char) that differ in [s1] and [s2].
    If [n] is zero, the return value is zero.

    #include <string.h>
    int strncmp(const char* s1, const char* s2, size_t n);
*/
int strncmp(const char* s1, const char* s2, size_t n)
{
    int sum = 0;

    for (size_t i = 0; i < n; i++)
    {
        sum = ((uint8_t*) s1)[i] - ((uint8_t*) s2)[i];
        if (sum || ((uint8_t*) s1)[i] == 0) break;
    }

    return sum;
}