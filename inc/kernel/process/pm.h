// Process Manager Header File // ~ eylon

#if !defined(PM_H)
#define PM_H

#include <kernel/fs.h>
#include <kernel/ui.h>
#include <libc/stdint.h>
#include <libc/stddef.h>
#include <libc/proc.h>
#include <libc/limits.h>

#define PM_DEFAULT_PRIORITY 20
#define PM_DEFUALT_CWD      "/"

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
    size_t pbrk;            // program break

    int pid;                // process id
    struct process* parent; // parent process pointer
    size_t child_count;     // child processes count

    pstatus_t status;       // process status
    int ticks;              // number of ticks to execute process
    size_t timestamp;       // time created
    int priority;           // process's scheduler priority

    fd_t* fds;              // process's file descriptors list
    struct tab* tab;        // process's tab
    int cwd;                // inode index of current working directory of the process

    void (*entry)();        // process entry point
    uint8_t exit_status;    // exit status

    char name[FNAME_MAX];   // process name

    struct process* next;
    struct process* prev;
} process_t;

process_t* pm_get_active();
size_t pm_brk(process_t* proc, size_t addr);
process_t* pm_load(process_t* parent, const char* path,  char* const argv[], int priority);
void pm_kill(process_t* proc);
int pm_get_pid();
int pm_ps(struct ps* psbuff, int count);
process_t* pm_get_process_by_id(int pid);
void init_pm();

#endif