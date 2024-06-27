// Terminal Command kill // ~ eylon

#include <stdio.h>
#include <stdlib.h>

/* Kill a process */
int main(int argc, char *argv[])
{
    int pid;

    if (argc < 2) {
        printf(" - Usage: %s [pid]\n", argv[0]);
        return -1;
    }
    
    pid = atoi(argv[1]);
    if (pid == 0 && argv[1][0] != '0') {
        printf(" - Invalid process ID. Aborting.\n");
        return -1;
    }

    if (kill(pid) != 0) {
        printf(" - Could not kill process %d.\n", pid);
        return -1;
    }
    printf(" - Process %d terminated.\n", pid);

    return 0;
}