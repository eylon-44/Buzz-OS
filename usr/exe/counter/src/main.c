// User Program counter.elf // ~ eylon

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

/* Count seconds up to the given argument. */
int main(int argc, char* argv[])
{
    size_t sec;

    // If there are not enough arguments
    if (argc < 2) {
        printf(" - Usage: counter [seconds]\n.");
        return -1;
    }
    // If argument is not a number
    if ((sec = atoi(argv[1])) == 0) {
        printf(" - Argument \"%s\" is not a number! Aborting.\n", argv[1]);
        return -1;
    }

    // Count
    printf("0\n");
    for (size_t i = 0; i < sec; i++) {
        milisleep(1000);
        printf("%d\n", i+1);
    }

    return 0;
}