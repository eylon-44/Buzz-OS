// Built-in Terminal Command clear // ~ eylon

#include <stdio.h>

// Clear the screen
int cmd_builtin_clear(int argc, char* argv[])
{
    printf("\f");

    return 0;
}