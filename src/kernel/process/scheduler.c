// Scheduler // ~ eylon

#include <kernel/process/scheduler.h>
#include <kernel/process/pm.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/mmlayout.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/heap.h>
#include <libc/stddef.h>
#include <libc/list.h>

// Scheduler's process queue
sched_queue_t queue = { .active=NULL, .proc_list=NULL, .count=0, .psum=0 };
// Scheduler's sleeped tasks linked list
static sleep_node_t* sleep_lst = NULL;


/* ~~~ Utils ~~~ */

// Step the queue; change the active process
static inline void step_queue()
{
    queue.active = queue.active->next;
    if (queue.active == NULL) queue.active = queue.proc_list;
}

// Get the currently active process
inline process_t* sched_get_active()
{
    return queue.active;
}

// Get a process by its ID
static process_t* get_process_by_id(int pid)
{
    process_t* p = queue.proc_list;
    
    // Iterate over the list
    while (p != NULL)
    {
        if (p->pid == pid) {
            return p;
        }
        p = p->next;
    }

    return NULL;
}

// Calculate and set the time slice for the active process
static void set_active_time_slice()
{
    size_t ticks = SCHED_CYCLE_TICKS * sched_get_active()->priority / queue.psum;
    // Ensure that the task gets at the minimum amount of execution time
    if (ticks < SCHED_MIN_TICKS) ticks = SCHED_MIN_TICKS;

    sched_get_active()->ticks = ticks;
}


/* ~~~ Sleep Queue ~~~ */

// Update sleeped tasks on scheduler tick
static void update_sleep()
{
    // If there are no sleeping tasks, return
    if (sleep_lst == NULL) return;

    sleep_lst->dticks--;              // tick

    // Wake up all the tasks that has finished sleeping
    while (sleep_lst != NULL && sleep_lst->dticks <= 0)
    {
        // Delete the task from the sleepy list and point onto the next one
        sleep_node_t* node = sleep_lst;                 // save the node in a temporary variable so we can free it from the heap
        sched_set_status(sleep_lst->proc, PSTATUS_READY);
        LIST_REMOVE_FRONT(sleep_lst);                   // remove the node from the list
        kfree(node);                                    // free the sleep node from the heap
    }
}

// Sleep a given process
void sched_sleep(int pid, size_t ticks)
{
    sleep_node_t* new_node = (sleep_node_t*) kmalloc(sizeof(sleep_node_t)); // allocate a new sleep node
    sleep_node_t* node     = sleep_lst;   // keep a duplicate of the original list so we can iterate over it

    new_node->proc   = get_process_by_id(pid);
    new_node->dticks = ticks;

    // Iterate over the list and insert the new sleep node into it while decrementing its ticks
    while (node != NULL)
    {
        /* If the delta ticks of the current node is larger than those of the the new node,
            insert the new node before of the current */
        if (node->dticks >= (int) ticks) {
            node->dticks -= ticks;  // decrement the ticks of the current node to not affect the other nodes by the change
            LIST_ADD_BEFORE(sleep_lst, new_node, node); // insert the new node before of the current one
            goto exit;
        }
        // Decrement the ticks in order to create a delta ticks list
        new_node->dticks -= node->dticks;
        node = node->next;
    }
    // This line will only execute if the list is empty or if this process is the longest to sleep in the list
    LIST_ADD_END(sleep_lst, new_node);

    exit:
        // Set process status to SLEEPED
        sched_set_status(new_node->proc, PSTATUS_SLEEPED);

        // If it is the active process that has asked to sleep, switch a task
        if (sched_get_active()->status == PSTATUS_SLEEPED) {
            sched_switch_next();
        }
        return;
}


/* ~~~ Task Management ~~~ */

/* Remove a process from the queue.
    Takes a pointer to the process structure in the queue and kills it. */
static void kill_process(process_t* proc)
{
    // Remove any sleep node associated with that process
    sleep_node_t* node = sleep_lst;
    while (node != NULL)
    {
        if (node->proc->pid == proc->pid) {
            LIST_REMOVE(sleep_lst, node);
        }
        node = node->next;
    }

    // Close open file descriptors
    fd_t* fd = proc->fds;
    while (fd != NULL) { fs_close(fd->fileno); }

    sched_set_status(proc, PSTATUS_DONE);    // set the status of the process to DONE and by that remove its priority from the queue's priority sum
    vmm_del_ctx(proc->cr3);             // delete the address space of the process
    LIST_REMOVE(queue.proc_list, proc); // remove it from the queue
    kfree(proc);                        // free the process structure
    queue.count--;                      // decrement the process count in the queue
}

// Update active process on scheduler tick
static void update_process()
{
    process_t* proc = sched_get_active();       // get active process
    proc->ticks--;                              // tick

    // If the current process`s ticks are less than or equal to 0, switch to the next READY process in the queue
    if (proc->ticks <= 0) {
        sched_switch_next();
    }
}

// Setup a task switch into a given process
void sched_switch(process_t* proc)
{
    sched_set_status(sched_get_active(), PSTATUS_READY);
    sched_set_status(proc, PSTATUS_ACTIVE);
    queue.active = proc;
}

// Setup a task switch into the next READY process
void sched_switch_next()
{
    // Set current process to READY if it was ACTIVE
    if (sched_get_active()->status == PSTATUS_ACTIVE) {
        sched_set_status(sched_get_active(), PSTATUS_READY);
    }

    // Step the queue until finding a READY process to execute. Kill all DONE processes.
    for (;;) {
        // Step the queue; change the active process
        step_queue();

        // Check if the active process should be terminated
        if (sched_get_active()->status == PSTATUS_DONE) {
            kill_process(sched_get_active());
        }
        // Check if the active process is READY to run
        else if (sched_get_active()->status == PSTATUS_READY) {
            break;
        }
    }

    sched_set_status(sched_get_active(), PSTATUS_ACTIVE);    // set the status of the active process to ACTIVE
    set_active_time_slice();                            // set the execution time slice for the active process
}

/* Add a process to the end of the queue.
    Takes a process data structure and returns a pointer to the new process in the queue.
    Returns a pointer to the new process data structure in the queue.
    The function sets the process as READY. */
process_t* sched_add_process(process_t proc)
{
    process_t* node;

    // Allocate memory for the new node
    node = (process_t*) kmalloc(sizeof(process_t));

    // Add the process to the queue
    *node = proc;
    LIST_ADD_END(queue.proc_list, node);

    // Set the status of the task to READY and by that add its priority to the queue's priority sum
    sched_set_status(node, PSTATUS_READY);
    // Increment the queue count
    queue.count++;

    return node;
}

// Update the status of a given thread; updates the queue's priority sum accordingly
void sched_set_status(process_t* proc, pstatus_t status)
{
    /* Status DONE cannot be changed. */
    if (proc->status == PSTATUS_DONE) return;

    /* If was READY or ACTIVE and is now changed to something that is neither READY nor ACTIVE,
        exclude the thread's priority from the queue's priority sum. */
    if ((proc->status == PSTATUS_ACTIVE || proc->status == PSTATUS_READY)
        && !(status == PSTATUS_ACTIVE || status == PSTATUS_READY)) 
    {
        queue.psum -= proc->priority;
    }
    /* If wasn'proc READY or ACTIVE and is now changed to something that is READY or ACTIVE,
        add the thread's priority to the queue's priority sum. */
    else if (!(proc->status == PSTATUS_ACTIVE || proc->status == PSTATUS_READY)
        && (status == PSTATUS_ACTIVE || status == PSTATUS_READY)) 
    {
        queue.psum += proc->priority;
    }

    // Set the status
    proc->status = status;
}

// Update the priority of a given thread
void sched_set_priority(process_t* proc, int priority)
{
    // If the thread is ACTIVE or READY, update the queue's priority sum accordingly
    if (proc->status == PSTATUS_ACTIVE || proc->status == PSTATUS_READY) {
        queue.psum += priority - proc->priority;
    }
    // Set the priority
    proc->priority = priority;
}


/* ~~~ Initiation & Event Handling ~~~ */

// Scheduler's tick event handler; being called uppon a timer interrupt
void sched_tick()
{
    update_sleep();
    update_process();
}

// Initiate the scheduler
void init_scheduler()
{
    /* Scheduler Initiation */
}