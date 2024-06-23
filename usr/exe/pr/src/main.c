// Terminal Command pr // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <proc.h>
#include <limits.h>

/* Change the priority of a process */
int main(int argc, char* argv[])
{
    int pid, priority;

    if (argc < 3) {
        printf(" - Usage: %s [pid] [priority]\n", argv[0]);
        return -1;
    }

    pid = atoi(argv[1]);
    if (pid == 0 && argv[1][0] != '0') {
        printf(" - Invalid PID. PID must be a non-negative number. Aborting.\n");
        return -1;
    }

    priority = atoi(argv[2]);
    if ((priority == 0 && argv[2][0] != '0') || priority < PRIORITY_MIN || priority > PRIORITY_MAX) {
        printf(" - Invalid priority. Priority must be number between %d and %d. Aborting.\n", PRIORITY_MIN, PRIORITY_MAX);
        return -1;
    }
    
    if (pr(pid, priority) == -1) {
        printf(" - Failed to change priorty.\n");
        return -1;
    }
    printf(" - Priority changed.\n");

    return 0;
}