/******************************************************************************************
    This is the only part of the operating system that wasn't written by me. I have just
    copy pasted it from somewhere and I have no idea how it works, but it does!
*******************************************************************************************/

#include <stdlib.h>
#include <stdint.h>

// Mersenne Twister parameters
#define MT_N 624
#define MT_M 397
#define MT_MATRIX_A 0x9908b0dfU
#define MT_UPPER_MASK 0x80000000U
#define MT_LOWER_MASK 0x7fffffffU

static uint32_t mt[MT_N];
static int mt_index = MT_N + 1;

static void generate_numbers()
{
    int i;
    uint32_t y;
    static const uint32_t mag01[2] = {0x0, MT_MATRIX_A};
    for (i = 0; i < MT_N - MT_M; i++) {
        y = (mt[i] & MT_UPPER_MASK) | (mt[i + 1] & MT_LOWER_MASK);
        mt[i] = mt[i + MT_M] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    for (; i < MT_N - 1; i++) {
        y = (mt[i] & MT_UPPER_MASK) | (mt[i + 1] & MT_LOWER_MASK);
        mt[i] = mt[i + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y = (mt[MT_N - 1] & MT_UPPER_MASK) | (mt[0] & MT_LOWER_MASK);
    mt[MT_N - 1] = mt[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1];
    mt_index = 0;
}

void srand(unsigned int seed)
{
    mt[0] = seed & 0xffffffffU;
    for (mt_index = 1; mt_index < MT_N; mt_index++) {
        mt[mt_index] = (1812433253U * (mt[mt_index - 1] ^ (mt[mt_index - 1] >> 30)) + mt_index);
    }
}

uint32_t rand()
{
    uint32_t y;
    if (mt_index >= MT_N) {
        if (mt_index == MT_N + 1) {
            srand(5489);
        }
        generate_numbers();
    }
    y = mt[mt_index++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680U;
    y ^= (y << 15) & 0xefc60000U;
    y ^= (y >> 18);
    return y;
}