// Scheduler // ~ eylon

#include <kernel/process/scheduler.h>
#include <kernel/process/pm.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/mmlayout.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/heap.h>
#include <libc/stddef.h>

// Scheduler's thread queue
squeue_t queue = { .thread_n=NULL, .count=0, .psum=0, .active=NULL };
// Scheduler's sleeped tasks linked list
static sleep_node_t* sleep_n = NULL;


/* ~~~ Utils ~~~ */

// Step the index in the queue
static inline void step_queue()
{
    queue.active = queue.active->next;
    if (queue.active == NULL) queue.active = queue.thread_n;
}

// Get the currently active thread
static inline thread_t* get_thread()
{
    return &queue.active->thread;
}

// Get a thread by its ID
static thread_t* get_thread_by_id(int pid, int tid)
{
    thread_node_t* t = queue.thread_n;
    
    // Iterate over the list
    while (t != NULL)
    {
        if (t->thread.pid == pid && t->thread.tid == tid) {
            return &t->thread;
        }
        t = t->next;
    }

    return NULL;
}

// Calculate time slice in ticks for the current task to execute
static size_t get_time_slice()
{
    size_t ticks = SCHED_CYCLE_TICKS * (get_thread()->priority / queue.psum);
    // Ensure that the task gets at the minimum amount of execution time
    if (ticks < SCHED_MIN_TICKS) ticks = SCHED_MIN_TICKS;
    return ticks;
}


/* ~~~ Sleep Queue ~~~ */

// Update sleeped tasks on scheduler tick
static void update_sleep()
{
    sleep_node_t tmp_n;

    // If there are no sleeping tasks, return
    if (sleep_n == NULL) return;

    sleep_n->dticks--;              // tick

    // Wake up all the tasks that has finished sleeping
    while (sleep_n != NULL && sleep_n->dticks <= 0)
    {
        // Delete the task from the sleepy list and point onto the next one
        tmp_n = *sleep_n;           // save the node in a temporary variable so we can free it from the heap
        kfree(sleep_n);             // free the sleep node from the heap
        sleep_n = tmp_n.next;       // point to the next task in the linked list
    }
}

// Sleep a given thread
void sched_sleep(int pid, int tid, size_t ticks)
{
    sleep_node_t* new_node = (sleep_node_t*) kmalloc(sizeof(sleep_node_t)); // allocate a new sleep node
    sleep_node_t* list     = sleep_n;   // keep a duplicate of the original list so we can iterate over it
    sleep_node_t* last     = new_node;  /* pointer to the node before the current in the list.
                                            default is set to new_node so that the value last->next
                                            could be overriden in the case that there is no last node (for
                                            example, when insereting the new node at the head of the list) */

    new_node->next = NULL;              // default value is used in the case that the list is empty

    // Iterate over the list and insert the new sleep node into it
    while (list != NULL)
    {
        /* If the delta ticks of the current node is larger than those of the the new node,
            insert the new node before of the current */
        if (list->dticks >= ticks) {
            list->dticks -= ticks;  // decrement the ticks of the current node to not affect the other nodes by the change
            // Insert the new node between his latter and former in the list
            last->next = new_node;  // in the case where there is no last node, the next line will override this value
            new_node->next = list;
            goto exit;
        }
        // Decrement the ticks in order to create a delta ticks list
        ticks -= list->dticks;

        // Store the current node as the last and point to the next node in the list
        last = list;
        list = list->next;
    }
    sleep_n = new_node;     // this line will only execute if the list is empty

    // Exit routine
    exit:
        new_node->pid    = pid;
        new_node->tid    = tid;
        new_node->dticks = ticks;

        // Set status to blocked
        get_thread_by_id(pid, tid)->status = TS_BLOCKED;

        // If it is the current running process that has asked to sleep, switch a task
        if (get_thread()->status == TS_BLOCKED) {
            sched_switch(get_thread());
        }
        return;
}


/* ~~~ Task Management ~~~ */

// Update currently running task on scheduler tick
static void update_task()
{
    thread_t* t = get_thread(); // current thread
    t->ticks--;                 // tick

    // If the current thread`s ticks are less than or equal to 0, perform a task switch
    if (t->ticks <= 0)
    {
        t->status = TS_READY;       // set current thread to READY (was ACTIVE)

        /* Step the queue until finding a READY thread to run.
            We can be sure that the loop won't loop forever as if there is no other
            READY thread to execute it will execute again the thread we have just switched from. */
        do {
            step_queue();
        } while(get_thread()->status == TS_READY);
        
        // Switch to that thread and set it as ACTIVE
        sched_switch(get_thread());
        get_thread()->status = TS_ACTIVE;
        get_thread()->ticks = get_time_slice();
    }
}

// Perform a task switch into a given thread
void sched_switch(UNUSED thread_t* t)
{
    // temp attach the current kstack and set ESP to it
    // switch a page directory
    // [before doing an iret change the stack]   
}

/* Add a thread to the end of the queue.
    Takes a thread data structure and returns a pointer to the new thread in the queue.
    The function assumes that [queue.active!=NULL] while being called. */
thread_t* sched_add_thread(thread_t t)
{
    thread_node_t* node = queue.active;

    // Get the last node from the queue
    while (node->next != NULL) {
        node = node->next;          // step the queue
    }
    // Allocate memory for the new node
    node->next = (thread_node_t*) kmalloc(sizeof(thread_node_t));

    // Set the node
    node->next->thread = t;
    node->next->next   = NULL;

    // Set the status of the task to READY and by that add its priority to the queue's priority sum
    sched_set_status(&node->next->thread, TS_READY);
    // Increment the queue count
    queue.count++;

    return &node->next->thread;
}

/* Remove a thread from the queue.
    Takes a pointer to the thread structure in the queue.
    The function assumes that [len(queue.thread_n)>1] while being called. */
void sched_rem_thread(thread_t* t)
{
    thread_node_t* node = queue.thread_n;
    thread_node_t* tmp;

    // Set the status of the task to DONE and by that remove its priority from the queue's priority sum
    sched_set_status(t, TS_DONE);

    // Get the node the comes before the node of the thread we would like to remove
    while (node->next != NULL) {
        if (&node->next->thread == t) {
            goto remove;            // found the thread, go to the code that removes it 
        }
        node = node->next;          // step the queue
    }
    // If reached this line, there is no such thread on the list, abort
    return;

    // Remove the node from the queue
    remove:
        tmp = node;

        // Remove the node from the queue
        node->next = node->next->next;

        // Deallocate the node from the heap
        kfree(tmp->next);

    // Decrement the queue count
    queue.count--;
}

// Update the status of the given thread
void sched_set_status(thread_t* t, tstatus_t status)
{
    /* If was READY or ACTIVE and is now changed to something that is neither READY nor ACTIVE,
        exclude the thread's priority from the queue's priority sum. */
    if ((t->status | TS_ACTIVE | TS_READY)
        && !(status | TS_ACTIVE | TS_READY)) 
    {
        queue.psum -= t->priority;
    }
    /* If wasn't READY or ACTIVE and is now changed to something that is READY or ACTIVE,
        add the thread's priority to the queue's priority sum. */
    else if (!(t->status | TS_ACTIVE | TS_READY)
        && (status | TS_ACTIVE | TS_READY)) 
    {
        queue.psum += t->priority;
    }

    // Set the status
    t->status = status;
}

// Update the priority of the given thread
void sched_set_priority(thread_t* t, int priority)
{
    // If the thread is ACTIVE or READY, update the queue's priority sum accordingly
    if (t->status == TS_ACTIVE || t->status == TS_READY) {
        queue.psum += priority - t->priority;
    }
    // Set the priority
    t->priority = priority;
}


/* ~~~ Initiation & Event Handling ~~~ */

// Scheduler's tick event handler; being called uppon a timer interrupt
void sched_tick()
{
    update_sleep();
    update_task();
}

// Initiate the scheduler
void init_scheduler()
{
    /* Scheduler Initiation */
}