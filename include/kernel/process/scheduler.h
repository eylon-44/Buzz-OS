// Scheduler Header File // ~ eylon

#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <kernel/process/pm.h>
#include <cpu/timer.h>

// Thread node type
typedef struct thread_node thread_node_t;
struct thread_node {
    thread_t thread;        // current
    thread_node_t* next;    // next
};

// Scheduler's queue type
typedef struct
{
    thread_node_t* thread_n;// pointer to the head of the threads linked list
    size_t count;           // thread count in list
    size_t psum;            // priority sum of all READY threads in queue
    thread_node_t* active;  // currently active thread from the linked list
} squeue_t;

// Scheduler's sleeping threads list type
typedef struct sleep_node sleep_node_t;
struct sleep_node {
    int pid;                // process ID
    int tid;                // thread ID
    size_t dticks;          // delta ticks
    sleep_node_t* next;     // next sleep node
};

/* Number of ticks in which a full queue cycle must be completed.
    By multiplying the TICK_HZ constant (the tick rate of the clock) with a certain value, we get a 
    constant value which indicates the number of ticks the clock has to commit in order for the 
    multiplayer's value in seconds to pass. Any change of the TICK_HZ value will affect the value stored 
    in SCHED_CYCLE_TICKS, but won't affect the actual time it takes for a full cycle to complete. */
#define SCHED_CYCLE_TICKS (0.25*TICK_HZ)
// Minimum ticks for a task to execute
#define SCHED_MIN_TICKS (0.01*TICK_HZ)

void sched_tick();
void sched_sleep(int pid, int tid, size_t ticks);
void sched_switch(thread_t* t);
thread_t* sched_add_thread(thread_t t);
void sched_rem_thread(thread_t* t);
void sched_set_status(thread_t* t, tstatus_t status);
void sched_set_priority(thread_t* t, int priority);
void init_scheduler();

#endif