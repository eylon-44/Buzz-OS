// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <libc/stdint.h>
#include <libc/stddef.h>

// Maximum number of processes allowed to run simultaneously
#define PM_MAX_PROCESSES 1024

// Thread status enum
typedef enum
{
    TS_NEW,         // new task is currently initiating
    TS_READY,       // task is ready to be picked by the scheduler and run
    TS_ACTIVE,      // task is currently running
    TS_BLOCKED,     // task is blocked
    TS_DONE,        // task is done and is about to be deleted
    TS_STOPPED      // task stopped until being continued
} status_t;

// Thread data structure
typedef struct thread thread_t ;
struct thread {
    int pid;            // process id - thread group id
    int tid;            // thread id
    thread_t* parnet;   // parent thread pointer
    size_t child_count; // child threads count

    uint32_t kesp;      // kernel stack pointer
    uint32_t cr3;       // CR3 register - address space

    status_t status;    // thread status
    uint32_t ticks;     // cpu ticks while the thread is active
    int priority;       // thread's scheduler priority
    int force_exit;     // set this to force exit of a process
    int exit_status;    // exit status
};

int pm_get_pid();
void init_pm();

#endif