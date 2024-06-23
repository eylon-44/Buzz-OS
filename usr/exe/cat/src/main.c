// Terminal Command cat // ~ eylon

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

/* Print a file. */
int main(int argc, char* argv[])
{
    struct stat statbuf;
    char path[PATH_MAX];
    char str_buff[256];
    int fd;

    if (argc < 2) {
        printf("Missing arguments. Aborting.\n");
        return -1;
    }

    // cat for each argument
    for (int i = 1; i < argc; i++)
    {
        realpath(argv[1], path);

        // Get the stat of the file
        if (stat(path, &statbuf) != 0) {
            printf("Cannot access \"%s\": No such file or directory.\n", path);
            return -1;
        }

        // If file is not a regular file
        if (statbuf.type != DT_REG) {
            printf("File %s is not a regular file. Aborting.\n", path);
            return -1;
        }
        
        // Open the file for reading
        fd = open(path, O_RDONLY);

        // Read and print until end of file
        str_buff[sizeof(str_buff)-1] = '\0';
        while (read(fd, str_buff, sizeof(str_buff)-1) != 0) { printf(str_buff); }
        
        // Close the file
        close(fd);
    }

    return 0;
}