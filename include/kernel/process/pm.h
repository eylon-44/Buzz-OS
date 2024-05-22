// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <kernel/fs.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// Maximum number of processes allowed to run simultaneously
#define PM_MAX_PROCESSES 1024

#define PM_TMP_INIT_PRC_DISK_OFFSET 512*60

// Thread status enum
typedef enum
{
    TS_NEW      = 0x01,     // new task is currently initiating
    TS_READY    = 0x02,     // task is ready to be picked by the scheduler and run
    TS_ACTIVE   = 0x04,     // task is currently running
    TS_BLOCKED  = 0x08,     // task is blocked
    TS_DONE     = 0x10,     // task is done and is about to be deleted
    TS_STOPPED  = 0x20      // task stopped until being continued
} tstatus_t;

// Thread data structure
typedef struct thread thread_t ;
struct thread {
    size_t cr3;             // CR3 register - address space
    size_t kesp;            // kernel stack pointer

    int pid;                // process id - thread group id
    int tid;                // thread id
    thread_t* parnet;       // parent thread pointer
    size_t child_count;     // child threads count

    tstatus_t status;       // thread status
    int ticks;              // cpu ticks while the thread is active
    int priority;           // thread's scheduler priority

    fd_t* fds;              // process's file descriptors list

    void (*entry)();        // thread entry function
    uint8_t exit_status;    // exit status
};

int pm_get_pid();
void init_pm();

#endif