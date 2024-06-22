// Freestanding Commands Manager // ~ eylon

#include <freestanding.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static cmd_freestanding_t* cmds_freestanding = NULL;
static size_t cmds_freestanding_count        = 0;

// Freestanding commands list
inline const cmd_freestanding_t* freestanding_get_cmds() { return cmds_freestanding; }
inline int freestanding_get_count() { return cmds_freestanding_count; }

/* Execute a freestanding program located at the bin directory. The function executes
    /bin/[argv[0]], passes [argv] to it, and returns the new process's PID. */
int freestanding_execute(char* argv[])
{
    int pid;
    char path[PATH_MAX] = BIN_PATH;
    strlcat(path, argv[0], PATH_MAX);

    pid = execve(path, argv);

    if (pid < 0) {
        printf("Failed to execute %s. Aborting.\n", path);
        return -1;
    }
    
    return pid;
}

// Scan the bin directory and add all of its programs as commands in the freestanding commands list
void init_freestanding()
{
    struct dirent entry;
    struct stat statbuf;

    // Open for reading and get the stat of the bin directory
    int fd = open(BIN_PATH, O_RDONLY | O_DIRECTORY);
    if (fd < 0 || stat(BIN_PATH, &statbuf) != 0) {
        printf("Failed to find %s. Aborting.\n", BIN_PATH);
        return;
    }

    // Allocate memory for the freestanding commands list
    cmds_freestanding       = (cmd_freestanding_t*) malloc(sizeof(cmd_freestanding_t) * statbuf.size);
    cmds_freestanding_count = statbuf.size;

    // Go over all files in the bin directory and add them to the freestanding commands list
    for (size_t i = 0; i < statbuf.size && read(fd, &entry, 1) != 0; i++)
    {
        size_t len = strlen(entry.d_name);
        cmds_freestanding[i].ref = malloc(len+1);
        strcpy(cmds_freestanding[i].ref, entry.d_name);
    }
    

    // Close the bin directory
    close(fd);
}