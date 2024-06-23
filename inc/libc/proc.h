// proc.h // ~ eylon

#if !defined(__LIBC_PROC_H)
#define __LIBC_PROC_H

#include <limits.h>

// Process status structure
struct ps
{
    char name[FNAME_MAX];       // process name
    int pid;                    // process ID
    int priority;               // process's scheduler priority
    int uptime;                 // time alive in miliseconds
};

int pr(int pid, int priority);
int ps(struct ps* psbuff, int count);

#endif