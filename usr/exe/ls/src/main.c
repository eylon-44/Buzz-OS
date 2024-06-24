// Terminal Command ls // ~ eylon

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>

/* List a directory or view file information. */
int main(int argc, char* argv[])
{
    struct dirent entry;
    struct stat statbuf;
    char path[PATH_MAX];

    // ls for each argument
    for (int i = 1; i < argc || i == 1; i++)
    {
        // Default the working directory
        if (argc < 2) {
            realpath(".", path);
        }
        else {
            realpath(argv[1], path);
        }

        // Get the stat of the file
        if (stat(path, &statbuf) != 0) {
            printf("Cannot access \"%s\": No such file or directory.\n", path);
            return -1;
        }

        // If file is a regular file
        if (statbuf.type == DT_REG) {
            printf("File: %s | Size: %d | Executable: %s\n", basename((const char*) path),
                statbuf.size, statbuf.exe ? "true" : "false");
        }
        // If file is a directory
        else if (statbuf.type == DT_DIR) {
            // Open the directory for reading
            int fd = open(path, O_RDONLY | O_DIRECTORY);

            printf("Dir: %s | File Count: %d\n", path, statbuf.size);

            // Go over all files in the directory and print them
            for (size_t i = 0; i < statbuf.size && read(fd, &entry, 1) != 0; i++)
            {
                printf(" - <%s> %s\n", entry.d_type == DT_REG ? "F" : "D", entry.d_name);
            }
            
            // Close the directory
            close(fd);
        }
    }

    return 0;
}