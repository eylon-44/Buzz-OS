// math/pow.c // ~ eylon

#include <math.h>

/* Get the absolute value of a number.

    #include <math.h>
    int abs(int j);
*/
inline int abs(int j)
{
    return j > 0 ? j : -j;
}