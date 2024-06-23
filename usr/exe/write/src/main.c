// Terminal Command write // ~ eylon

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <string.h>

/* Write to a file. */
int main(int argc, char* argv[])
{
    struct stat statbuf;
    char path[PATH_MAX];
    int fd, whence;

    if (argc < 4) {
        printf(" - Usage: %s [filepath] [-w/-a](write/append) [data...\n", argv[0]);
        return -1;
    }

    // Define writing mode
    if (strcmp(argv[2], "-w") == 0) {
        whence = SEEK_SET;
    }
    else if (strcmp(argv[2], "-a") == 0) {
        whence = SEEK_END;
    }
    else {
        printf(" - Flag argument must be \"-w\" (for writing) or \"-a\" (for appending).\n");
    }

    realpath(argv[1], path);

    // Get the stat of the file
    if (stat(path, &statbuf) != 0) {
        printf(" - Cannot access \"%s\": No such file or directory.\n", path);
        return -1;
    }

    // If file is not a regular file, abort
    if (statbuf.type != DT_REG) {
        printf(" - File \"%s\" is not a regular file. Aborting.\n", path);
        return -1;
    }
    
    // Open the file for writing and seek it; if [whence] is set to SEEK_SET truncate the file
    fd = open(path, O_WRONLY);
    lseek(fd, 0, whence);
    if (whence == SEEK_SET) ftruncate(fd, 0);
    
    // Write to the file
    for (int i = 3; i < argc; i++)
    {
        write(fd, argv[i], strlen(argv[i]));
        if (i != argc-1) {
            write(fd, " ", 1);
        }
    }
    
    // Close the file
    close(fd);

    return 0;
}