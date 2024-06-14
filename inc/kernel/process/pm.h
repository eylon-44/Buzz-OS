// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <kernel/fs.h>
#include <kernel/ui.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

// [DEBUG] REMOVE
#define PM_TMP_INIT_PRC_DISK_OFFSET 512*60

// Process status enum
typedef enum
{
    PSTATUS_NEW      = 0x01,     // process is initiating
    PSTATUS_READY    = 0x02,     // process is ready to be picked by the scheduler and run
    PSTATUS_ACTIVE   = 0x04,     // process is currently running
    PSTATUS_SLEEPED  = 0x08,     // process is blocked
    PSTATUS_DONE     = 0x10,     // process is done and is about to be deleted
    PSTATUS_BLOCKED  = 0x20      // process stopped until being continued
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
    struct tab* tab;        // process's tab

    void (*entry)();        // process entry point
    uint8_t exit_status;    // exit status

    struct process* next;
    struct process* prev;
} process_t;

process_t* pm_get_active();
process_t* pm_load(process_t* parent, const char* path, int priority);
void pm_kill(process_t* proc);
int pm_get_pid();
void init_pm();

#endif