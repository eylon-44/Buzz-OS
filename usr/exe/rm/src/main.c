// Terminal Command rm // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>

/* Remove a file or a directory from the file system. */
int main(int argc, char* argv[])
{
    char path[PATH_MAX];
    struct stat statbuff;

    if (argc < 2) {
        printf("Missing arguments. Aborting.\n");
        return -1;
    }

    // rm for each argument
    for (int i = 1; i < argc; i++)
    {
        realpath(argv[i], path);
        
        if (stat(path, &statbuff) != 0) {
            printf(" - Could not delete file at \"%s\". Aborting.\n", path);
            return -1;
        }

        // If file is a regular file
        if (statbuff.type == DT_REG) {
            unlink(path);
            printf(" - Removed file at \"%s\".\n", path);
        }
        // If file is a directory
        else if (statbuff.type == DT_DIR) {
            rmdir(path);
            printf(" - Removed directory at \"%s\".\n", path);
        }
    }

    return 0;
}