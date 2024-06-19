// Scheduler Header File // ~ eylon

#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <kernel/process/pm.h>
#include <drivers/timer.h>

// Scheduler's queue structure
typedef struct
{
    process_t* active;      // currently running process
    process_t* proc_list;   // process list
    size_t count;           // ACTIVE process count in list
    size_t psum;            // priority sum of all READY processes in queue
} sched_queue_t;

// Scheduler's sleeping processes list structure
typedef struct sleep_node {
    process_t* proc;        // sleeping process
    int dticks;             // delta ticks

    struct sleep_node* next;
    struct sleep_node* prev;
} sleep_node_t;

/* Number of ticks in which a full queue cycle must be completed.
    By multiplying the TIMER_TICK_HZ constant (the tick rate of the clock) with a certain value, we get a 
    constant value which indicates the number of ticks the clock has to commit in order for the 
    multiplayer's value in seconds to pass. Any change of the TIMER_TICK_HZ value will affect the value stored 
    in SCHED_CYCLE_TICKS, but won't affect the actual time it takes for a full cycle to complete. */
#define SCHED_CYCLE_TICKS (0.05*TIMER_TICK_HZ)
// Minimum ticks for a task to execute
#define SCHED_MIN_TICKS (0.01*TIMER_TICK_HZ)

void sched_tick();
void sched_sleep(int pid, size_t ticks);
process_t* sched_add_process(process_t t);
void sched_set_status(process_t* t, pstatus_t status);
void sched_set_priority(process_t* t, int priority);
void sched_switch(process_t* t);
void sched_switch_next();
process_t* sched_get_active();
void init_scheduler();

#endif