// Terminal Command ps // ~ eylon

#include <stdio.h>
#include <stdlib.h>
#include <proc.h>

/* List running processes. */
int main(int argc, char* argv[])
{
    struct ps* psbuff;
    int count;

    count = ps(NULL, 0);
    psbuff = (struct ps*) malloc(sizeof(struct ps) * count);
    ps(psbuff, count);

    // Print process information
    printf(" - %d running processes.\n", count);
    printf(" PID    PRIORITY    UPTIMEsec    NAME\n");
    for (int i = 0; i < count; i++)
    {
        char pidstr[16];
        char prioritystr[16];
        char uptimestr[16];

        itoapad(psbuff[i].pid, pidstr, 3);
        itoapad(psbuff[i].priority, prioritystr, 3);
        itoapad(psbuff[i].uptime/1000, uptimestr, 6);

        printf(" %s    %s         %s       %s\n", pidstr, prioritystr, uptimestr, psbuff[i].name);
    }

    free(psbuff);

    return 0;
}