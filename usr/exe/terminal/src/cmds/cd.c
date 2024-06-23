// Built-in Terminal Command cd // ~ eylon

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

// Extern [wd] string from main.c
extern char wd;

// Change the current working directory
int cmd_builtin_cd(int argc, char* argv[])
{
    char path[PATH_MAX];

    if (argc < 2) {
        printf("Invalid use of command.\n");
        return -1;
    }

    realpath(argv[1], path);

    if (chdir(path) != 0) {
        printf("No such directory.\n");
        return -1;
    }
    
    getcwd(&wd, PATH_MAX);

    return 0;
}