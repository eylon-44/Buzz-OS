// Terminal Command echo // ~ eylon

#include <stdio.h>

/* Print to the screen */
int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        printf("%s%s", argv[i], i == argc-1 ? "" : " ");
    }
    printf("\n");
}