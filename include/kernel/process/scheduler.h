// Scheduler Header File // ~ eylon

#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include <kernel/pm.h>

typedef struct
{
    size_t index;           // queue thread selector; cycles over the queue
    size_t count;           // thread count in queue
    size_t psum;            // priority sum of all READY threads in queue
    thread_t* thread_q;     // thread queue
} queue_t;

#define CYCLE_TICKS 2000    // number of ticks in which a full queue cycle must be completed

#endif