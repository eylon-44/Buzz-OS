// User Program counter.elf // ~ eylon

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Count seconds up to the given argument. */
int main(int argc, char* argv[])
{
    int sec;

    // If there are not enough arguments
    if (argc < 2) {
        printf(" - Usage: %s [seconds]\n.", argv[0]);
        return -1;
    }
    // If argument is not a number
    if ((sec = atoi(argv[1])) <= 0) {
        printf(" - Argument \"%s\" is invalid. Aborting.\n", argv[1]);
        return -1;
    }

    // Count
    for (size_t i = 0; i < sec; i++) {
        printf("%d\n", sec-i);
        milisleep(1000);
    }
    printf("TIME'S UP!\n");

    return 0;
}