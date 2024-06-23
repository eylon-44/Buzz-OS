// Terminal Command file // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

/* Create a file. */
int main(int argc, char* argv[])
{
    char path[PATH_MAX];
    int fd;


    if (argc < 2) {
        printf(" - Usage: %s [filename]\n", argv[0]);
        return -1;
    }

    // file for each argument
    for (int i = 1; i < argc; i++)
    {
        realpath(argv[i], path);

        fd = creat(path);

        if (fd < 0) {
            printf(" - Could not create new file at \"%s\". Aborting.\n", path);
            return -1;
        }
        printf(" - Created new file at \"%s\".\n", path);

        close(fd);
    }

    return 0;
}