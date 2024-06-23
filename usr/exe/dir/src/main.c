// Terminal Command dir // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

/* Create a directory. */
int main(int argc, char* argv[])
{
    char path[PATH_MAX];
    int fd;


    if (argc < 2) {
        printf("Missing arguments. Aborting.\n");
        return -1;
    }

    // dir for each argument
    for (int i = 1; i < argc; i++)
    {
        realpath(argv[i], path);

        fd = mkdir(path);

        if (fd < 0) {
            printf(" - Could not create new directory at \"%s\".\n", path);
            return -1;
        }
        printf(" - Created new directory at \"%s\".\n", path);

        close(fd);
    }

    return 0;
}