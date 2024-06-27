// math/pow.c // ~ eylon

#include <math.h>

/* Returns ]x] raised to the power of [y].

    #include <math.h>
    int pow(int x, int y);
*/
int pow(int x, int y)
{
    int res = 1;

    for (int i = 0; i < y; i++) {
        res *= x;
    }

    return res;
}