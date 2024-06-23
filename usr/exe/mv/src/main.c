// Terminal Command mv // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* Change the name or location of a file. */
int main(int argc, char* argv[])
{
    char oldpath[PATH_MAX];
    char newpath[PATH_MAX];

    if (argc < 3) {
        printf("Missing arguments. Aborting.\n");
        return -1;
    }

    realpath(argv[1], oldpath);
    realpath(argv[2], newpath);

    if (rename(oldpath, newpath) != 0) {
            printf(" - Could not move \"%s\" to \"%s\".\n", oldpath, newpath);
            return -1;
    }
    printf(" - Moved \"%s\" to \"%s\".\n", oldpath, newpath);

    return 0;
}