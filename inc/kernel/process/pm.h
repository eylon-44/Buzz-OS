// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <kernel/fs.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// [DEBUG] REMOVE
#define PM_TMP_INIT_PRC_DISK_OFFSET 512*60

// Process status enum
typedef enum
{
    TS_NEW      = 0x01,     // process is initiating
    TS_READY    = 0x02,     // process is ready to be picked by the scheduler and run
    TS_ACTIVE   = 0x04,     // process is currently running
    TS_BLOCKED  = 0x08,     // process is blocked
    TS_DONE     = 0x10,     // process is done and is about to be deleted
    TS_STOPPED  = 0x20      // process stopped until being continued
} pstatus_t;

// Process data structure
typedef struct process {
    size_t cr3;             // CR3 register - address space
    size_t kesp;            // kernel stack pointer

    int pid;                // process id
    struct process* parnet; // parent process pointer
    size_t child_count;     // child processes count

    pstatus_t status;       // process status
    int ticks;              // number of ticks to execute process
    int priority;           // process's scheduler priority

    fd_t* fds;              // process's file descriptors list

    void (*entry)();        // process entry point
    uint8_t exit_status;    // exit status

    struct process* next;
    struct process* prev;
} process_t;

int pm_get_pid();
void init_pm();

#endif