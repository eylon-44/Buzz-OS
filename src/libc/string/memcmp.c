// string/memcmp.c // ~ eylon

#include <string.h>
#include <stddef.h>
#include <stdint.h>

/*  Compare memory areas.

    The memcmp() function compares the first [n] bytes (each interpreted as 
    unsigned char) of the memory areas [s1] and [s2].

    The memcmp() function returns an integer less than, equal to, or greater 
    than zero if the first [n] bytes of [s1] is found, respectively, to be less
    than, to match, or be greater than the first [n] bytes of [s2]. For a nonzero 
    return value, the sign is determined by the sign of the difference between 
    the first pair of bytes (interpreted as unsigned char) that differ in [s1] and [s2].
    If [n] is zero, the return value is zero.

    #include <string.h>
    int memcmp(const void* s1, const void* s2, size_t n);
*/
int memcmp(const void* s1, const void* s2, size_t n)
{
    int sum = 0;

    for (size_t i = 0; i < n; i++)
    {
        sum = ((uint8_t*) s1)[i] - ((uint8_t*) s2)[i];
        if (sum) break;
    }

    return sum;
}