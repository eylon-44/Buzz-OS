# Process Manager

The Buzz-OS is a multi-processing system. It allows multiple processes run simultaneously while not interrupting or getting interrupted by other processes.

### Task Switching
A task switch is about saving the last context and loading a new one. A context includes the following attributes:

* EAX, EBX, ECX, EBP, EDI, ESI, EDX registers
* SS:ESP, CS:EIP, EFLAGS registers
* CR3 register (address space)
* Kernel's ESP

In Buzz, each process has its own address space; in that address space, the first 3GB of memory belongs to the user, and the last 1GB for the kernel. Across all address spaces, the kernel's memory layout stays the same, except for the kernel's stacks area (and the last 4MB of memory, but that's a story for another time). The kernel's stack changes with each address space, or put simply, each user thread has its own kernel stack stored in its own address space. That way, a kernel stack is where information regarding a single particular thread is being stored.

The only way to transition from user space to kernel space, is by using an interrupt. When an interrupt occurs, either by hardware or by a user instruction, the following happens:


* The user's SS:ESP, EFLAGS and CS:EIP registers (possibly with an error code) are being pushed into the kernel's stack (SS0:ESP0) in that order
* SS:ESP is set with the SS0:ESP0 values (defined in the TSS)
* CS:EIP is set with the address of the kernel's interrupt handler function (defined in the IDT)

All of the actions above are being executed by hardware and result in a jump to the kernel space while pushing some of the user's context into the kernel's stack.

- [ ] EAX, EBX, ECX, EBP, EDI, ESI, EDX registers
- [X] SS:ESP, CS:EIP, EFLAGS registers
- [ ] CR3 register (address space)
- [ ] Kernel's ESP

Now at the kernel, we can save all of the user's general purpose registers by using the PUSHAD instruction, which pushes the EAX, ECX, EDX, EBX, original ESP, EBP, ESI, and EDI registers onto the kernel's stack.

- [X] EAX, EBX, ECX, EBP, EDI, ESI, EDX registers
- [X] SS:ESP, CS:EIP, EFLAGS registers
- [ ] CR3 register (address space)
- [ ] Kernel's ESP

Next, we need to save the value of the CR3 register, which is a pointer to the physical base address of the current address space. Saving this value in the stack won't do; the stack is technically only available for the current context, while the CR3 value will only be used by other contexts. Moreover, as this value is only given once at the creation of each process and later remains unmodified, there is no need to save it anew each time an interrupt from the user occurs. Instead, we will save it in a thread data structure located in the heap; thus it can be accessed within all address spaces.

- [X] EAX, EBX, ECX, EBP, EDI, ESI, EDX registers
- [X] SS:ESP, CS:EIP, EFLAGS registers
- [X] CR3 register (address space)
- [ ] Kernel's ESP

Lastly, the kernel's stack pointer. When the kernel switches to a different address space, the stack changes, but ESP stays the same - still pointing to the bottom of the old stack. In order to fix that, we can update ESP to point at the bottom of the new stack we have just switched to. But what is that bottom? Well, that bottom is the currently-running-kernel-thread's ESP value at the moment before it switched to a different address space. Similar to CR3, this, of course, serves no purpose being saved in the stack; as this value indicates where the stack is, without it we could not locate the stack nor the pointer to it because it is being stored in the stack... So a moment before the kernel switches to a different address space it shall store its current ESP value in the thread data structure located at the heap.

- [X] EAX, EBX, ECX, EBP, EDI, ESI, EDX registers
- [X] SS:ESP, CS:EIP, EFLAGS registers
- [X] CR3 register (address space)
- [x] Kernel's ESP

Great, we have now saved all the information we need to describe the current context. The next step, as already mentioned, is to load the next context.

We can see now that most of the context of a certain thread is saved in the kernel's stack of that thread, and the rest of it - the part we need to access globaly - at the heap.

If the kernel would like to return back to the caller at ring 3, all it has to do is to undo the first 2 steps we have taken.

* POPAD instruction - pop all general purpose registers
* IRETD instruction - undo INT instruction: pop CS:EIP, EFLAGS, SS:ESP

If we would like to switch a context - to return, instead to the calling process, to a different one - we shall first save our current ESP, so later we could get back to where we have left off; change the address space to that of the destination process; and restore its kernel's ESP. Now, the address space have changed, meaning there resides a different process in the user space; and the kernel's stack have changed, meaning it now holds a different context, so when we try to return back to ring 3, we will load back a different context.

Essentially, you switch a context by returning from an interrupt loading in a different set of registers than what the interrupt was originally called with.

Note that when an interrupt from ring 0 occurs, everything behaves the same except that SS:ESP is unchanged. It is not being pushed by the interrupt, not being restored by the IRET instruction, and not being set with the SS0:ESP0 values at the moment of the interrupt.

To better illustrate what we have got so far:

```nasm
;; i386 nasm pseudo code

;; User Thread 4 ;;
int 0x80

;; Hardware ;;
mov [ss0:esp0+0],  ss
mov [ss0:esp0+4],  esp
mov [ss0:esp0+8],  eflags
mov [ss0:esp0+12], cs
mov [ss0:esp0+16], eip
mov [ss0:esp0+20], error_code

mov ss, ss0
mov esp, esp0

mov cs, 0
jmp kernel_interrupt_handler

;; Kernel ;;
pushad

; ...context..switch... ;
mov [thread4_data.esp], esp
mov cr3, [thread12_data.cr3]
mov esp, [thread12_data.esp]

popad
iretd

;; Hardware [pseudo code] ;;
mov ss,     [ss0:esp0+0]
mov esp,    [ss0:esp0+4]
mov eflags, [ss0:esp0+8]
mov cs,     [ss0:esp0+12]
mov eip,    [ss0:esp0+16]

;; User Thread 12 ;;
...
```

As there is only one thread per process for now, there is no need to update the TSS each task switch, only to initiate it with the top address of the kernel stack. Later, if we will have multiple threads per process, we will have to set SS0:ESP0 with the kernel stack of the thread we are currently switching to before jumping to it each task switch.

The thread data structure:

#### Thread
```c
typedef struct {
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
} thread_t;
```

Each task within the system has a status which describes what operations are allowed on the task. The initial status of a task after creation is READY. If a task's state is READY, it is visible by the scheduler and may be chosen for execution. When the CPU is given to a task, the task's status switches from READY to ACTIVE. If the execution of a task needs to be suspended (e.g. for sleeping or waiting for an IO operation), its status is BLOCKED. When the execution of a task is completed, its status becomes DONE before it is eventually deleted along with all its associated data structures. A task which is currently running can process a SIGSTOP signal which stops the task until it receives a SIGCONT signal.

```c
typedef enum
{
    TS_NEW,         // new task is currently initiating
    TS_READY,       // task is ready to be picked by the scheduler and run
    TS_ACTIVE,      // task is currently running
    TS_BLOCKED,     // task is blocked
    TS_DONE,        // task is done and is about to be deleted
    TS_STOPPED
} status_t;
```

> [!Note]
A note about interrupts: interrupts that occur while interrupts are disabled (after using the CLI instruction) will be pending until interrupts are enabled again to fire (after using the STI) instruction.


### The Task State Segment

The **Task State Segment (TSS)** is a data structure originally designed to be used with hardware task-switching, where each individual task has its own TSS. But because software task-switching it faster and more flexible, the TSS is rarely being used for its original purpose. The only reason for still using the TSS, is to transition from ring 3 (user space) to ring 0 (kernel space). When doing so, the CPU will look at the TSS structure (described below) and will load from it the `esp0` attribute into the ESP register and the `ss0` attribute into the SS register; there is no way around it.

The TSS data structure:

```c
typedef struct {
	uint32_t prev_tss;      // unused
	uint32_t esp0;          // stack pointer to load when changing to ring 0
	uint32_t ss0;           // stack segment to load when changing to ring 0
	uint32_t esp1;          // unused
	uint32_t ss1;           // unused
	uint32_t esp2;          // unused
	uint32_t ss2;           // unused
	uint32_t cr3;           // unused
	uint32_t eip;           // unused
	uint32_t eflags;        // unused
	uint32_t eax;           // unused
	uint32_t ecx;           // unused
	uint32_t edx;           // unused
	uint32_t ebx;           // unused
	uint32_t esp;           // unused
	uint32_t ebp;           // unused
	uint32_t esi;           // unused
	uint32_t edi;           // unused
	uint32_t es;            // unused
	uint32_t cs;            // unused
	uint32_t ss;            // unused
	uint32_t ds;            // unused
	uint32_t fs;            // unused
	uint32_t gs;            // unused
	uint32_t ldt;           // unused
	uint16_t trap;          // unused
	uint16_t iomap_base;    // unused
} __attribute__((packed)) tss_t;
```

The TSS initiation process includes creating a segment for it in the GDT and loading it into the TSS register in the following manner:

> [!Note]
Some of the GDT entry fields do not match to those of the TSS entries.

<br> The segment's base is set with the base address of the TSS; the limit with the size of the TSS minus one; DPL is set to 0; the access options region is set to 0x89, which translates to `TSS segment, 32-bit, DPL 0;` and the flags are kept zero.

```c
// create a gdt entry
#define GDT_ENTRY(base, limit, dpl, access_options, flags_options) { ... }

// tss segment
GDT_ENTRY(&TSS, sizeof(TSS)-1, 0, 0x89, 0)
```

<br> Next, initiate the TSS structure - set the appropriate `esp0` and `ss0` values and zero out the other fields.


```c
tss_t* tss_entry = MM_TSS_START;

memset((void*) tss_entry, 0, sizeof(tss_entry));
tss_entry.esp0 = KERNEL_STACK_ADDRESS;
tss_entry.ss0  = KERNEL_DATA_SEGMENT;
```

<br> 
Lastly, a TSS flush (load) function to commit our changes:

```gas
flush_tss:
	movw $TSS_SEG_SELCTOR ,%ax
	ltr %ax
	ret
```


### Loading a Process

Following are the steps for loading a process:

* Merge the kernel thread with the calling thread
    - Enable interrupts

* Create a new page directory for the process
    - Allocate a page
    - Initiate it with zeros
    - Copy the higher half kernel's page tables into it
    - Allocate and map a kernel stack for the process

* Load the process into memory
    - Load the process' ELF from disk into a scratch space in memory
    - Parse the ELF into the address space

* Queue the process in the scheduler
    - Allocate a thread data structure in the heap and add it into the thread list
    - Initiate its data structure

The kernel thread merging ensures that the loader function won't block the entire system for a long period of time when loading a large programs. It places the loader function in the scheduler's queue instead of the waiting caller.

| Thread                                           | Action                        |
|--------------------------------------------------|-------------------------------|
| user thread 01                                   | clone syscall                 |
| user thread 01 (in kernel loader function)       | task switch                   |
| user thread 15                                   | task switch                   |
| user thread 38                                   | task switch                   |
| user thread 01 (in kernel loader function)       | finished loading process      |
| user thread 01 (in user space)                   | task switch                   |
| user thread 38                                   | task switch                   |
| user thread 01 (in user space)                   | task switch                   |
| user thread 05 (new thread created by thread 01) | ...                           |


### The Init Process

A kernel's main function usually ends along the lines of:
```C
asm("sti");
for(;;) asm("hlt");
```

From that moment on, interrupts are enabled, the timer ticks and the scheduler kicks in and starts switching between all of the threads in its queue. The problem is that at this point, the scheduler still has no threads in its queue, meaning that the user has no interface creating a thread on its own. To solve that, the kernel is going to start the first user process itself. This process is called the **init process** and it is a user space daemon which bootstraps the user space and supply the user its first interface. The exact responsibility of the init process is described in [another docs](INITD.md), but as for the process manager, its job is to load the init process at startup as the first process.


### Process Destruction <span style="color: gray; font-weight: lighter; font-size: 10px;">(imagine a lot of explosions)</span>

* Delete the page direcotry stored in physical address of thread_t.cr3
* Deallocate kernel stack
* Destory children if present
* Delete the thread data structure from the scheduler's queue


### Scheduling

The scheduling algorithm is the algorithm which dictates how much CPU time is allocated to each thread. The scheduler must make sure that no task is starving - all tasks must get their CPU time; devide CPU resources based on priorities - high-priority tasks should get more CPU time than low-priority ones; and scale well with a growing number of tasks - ideally being O(1).

The scheduler's queue is a list of all of the threads in the system. The algorithm cycles over the queue and calculate for each thread an execution time based on its priority and the total number of threads in the system. The queue can be defined as follows:

```c
typedef struct
{
    size_t index;           // queue thread selector; cycles over the queue
    size_t count;           // thread count in queue
    size_t psum;            // priority sum of all READY threads in queue
    thread_t* thread_q;     // thread queue
} queue_t;

#define CYCLE_TICKS 2000    // number of ticks in which a full queue cycle must be completed
```

`index` keeps track about each thread in the queue is currently being executed; `count` is the number of threads in the queue with a `status` value of `READY`; `psum` is the sum of all `READY` threads' priority value; `thread_q` is the list of threads. The constant value `CYCLE_TICKS` defines in how many ticks the scheduler has the complete a full queue cycle. Using these values we can calculate the amount of time each thread should run per cycle with the following equation:

> time_slice = CYCLE_TICKS * (queue.thread_q[queue.index].priority / queue.psum)

Note that threads that are not in the `READY` condition will be ignored by the scheduler and won't affect the `queue.count` and `queue.psum` values.

The timer raises an interrupt every fixed amount of time (defined in the [timer driver docs](TIMER.md)). When a timer callback occurs, the scheduler increases the `ticks` field of the currently running thread by one and checks if it has passed the `time_slice` value calculated above. If it did, the scheduler resets the `ticks` field of the current thread, increases the `index` value in its queue by one (or reseting it if completed a cycle) and by that moves onto the next thread in the queue, and performs a context switch into the new thread; else, it returns back to the interrupted thread.


### Sleeping

A thread can choose sleep for a certain amount of time. When doing so, its status is set to `BLOCKED`, causing the scheduler to fully ignore it, and by that it turns inactive. 

HZ 100ms

SLEEP 2000ms

TICKS SLEEP/HZ

We keep track of sleeping threads by using a linked delta queue sleep event list. Each node in the list includes the thread's ID information, a delta amount of ticks between the thread and the threads before of it in the list to sleep, and a pointer to the next node.

```c
typedef struct sleep_node sleep_node_t;

struct sleep_node {
    int pid;                // process ID
    int tid;                // thread ID
    int dticks;             // delta ticks
    sleep_node_t* next;     // next sleep node
};
```

To illustrate, say 5 different threads have requested from the kernel to sleep for different periods of time. Thread A requested to sleep for 4 ticks, thread B for 6, thread C for 6 as well, thread D for 12 and thread E for 16. The list will look as follows:

> {A, 4} -> {B, 2} -> {C, 0} -> {D, 6} -> {E, 4}

Now, at each clock tick, the scheduler will decrement the `dticks` value of the first thread in the list, which in our example is thread A.

> {A, 3} -> {B, 2} -> {C, 0} -> {D, 6} -> {E, 4}

After 3 more ticks, A's `dticks` will be equal to 0; at that point, A has finished sleeping so we delete it from the list and set its thread status back to `ACTIVE`, so the scheduler will pick it.

> {B, 2} -> {C, 0} -> {D, 6} -> {E, 4}

Now, each tick will cause thread B's `dticks` value to decrement until hitting 0. So after 2 ticks from now

> {B, 0} -> {C, 0} -> {D, 6} -> {E, 4}

we should wake up both threads B and C, as their `dticks` values are both 0.

> {D, 6} -> {E, 4}

If now thread has A requested to sleep for 8 ticks we will insert it after thread D.

> {D, 6} -> {A, 2} -> {E, 4}


### Event Control Block
[...]


### IO Blocking
there is a linux syscall for this, i have used this when i played around with assembly, but how does it work?


### Signals


### The User Interface

<!-- 
INT     https://www.felixcloutier.com/x86/intn:into:int3:int1 
IRET    https://www.felixcloutier.com/x86/iret:iretd:iretq
SLEEP   https://wiki.osdev.org/Blocking_Process

GIVE UP CPU TIME https://man7.org/linux/man-pages/man2/sched_yield.2.html
SYSCALLS    https://stackoverflow.com/questions/64114316/why-do-we-need-a-wait-system-call
            https://man7.org/linux/man-pages/man2/waitid.2.html
-->