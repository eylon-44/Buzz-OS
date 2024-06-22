// Built-in Terminal Command pwd // ~ eylon

#include <stdio.h>

// Extern [wd] string from main.c
extern char wd;

// Print the currently working directory
int cmd_builtin_pwd(int argc, char* argv[])
{
    printf("Working directory: %s\n", &wd);
    return 0;
}