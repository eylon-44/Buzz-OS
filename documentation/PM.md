# Process Manager

### Goals of a Process Manager

* Spawning and killing processes and threads
* Supporting a jump from ring 0 to 3 and vice versa
* Scheduling thread execution


### Programs, Processes & Threads

A **program** is a set of instructions that a computer can execute. When we write a C or an Assembly program and compile it, the compiler produces binary code. This compiled binary code is a program. Only when we execute that program, it becomes a **process**. You can think of a process as an ‘active’ entity, opposed to a program which is a ‘passive’ entity. A **thread** is a single flow of control (code) within a process. It is where execution takes place. A process is essentially a group of threads that share the same resources, including address space, code, global data and open file streams, but a different stack. A process must own at least one thread; with no threads, a process won't be able to execute anything and therefore be useless. That means that process creation includes the act of creating at least one thread for that process. By having multiple threads, a process can execute multiple tasks concurrently.


### Context Switching & The Task State Segment

A **context** describes a collection of attributes regarding a thread's execution environment. These attributes may include a virtual address space, registers (e.g. eax, eip, esp, eflgas) and a kernel stack. In other words, a context describes all attributes that may differ in one running process from another.

As all code execution (in this OS) takes place on a single processor, there could only be one thread runnnig at any given moment. In order for the operating system to achieve a multitasking effect, we can switch between different contexts every short period of time, and by doing so giving each thread its own time splice for executing.

The **Task State Segment (TSS)** is a data structure originally designed to be used with hardware task-switching, where each individual task has its own TSS. But because software task-switching it faster and more flexible, the TSS is rarely being used for its original purpose. The only reason for still using the TSS, is to transition from ring 3 (user space) back to ring 0 (kernel space). When doing so, the CPU will look at the TSS structure (described below) and will load from it the `esp0` attribute into the ESP register and the `ss0` attribute into the SS register; there is no way around it.

The TSS initiation process includes creating a segment for it in the GDT in the following manner:

> [!Note]
Some of the GDT entry fields do not match to those of the TSS entries.

<br> The segment's base is set with the base address of the TSS, the limit with the size of the TSS minus one, DPL is set to 0, the access options region is set to 0x89, which translates to `TSS segment, 32-bit, DPL 0`, and the flags are kept zero.

```c
// create a gdt entry
#define GDT_ENTRY(base, limit, dpl, access_options, flags_options) { ... }

// tss segment
GDT_ENTRY(&TSS, sizeof(TSS)-1, 0, 0x89, 0)
```

<br> For easier understanding, it can also be put this way:

```c
tss_t tss_entry;
gdt_t g;

uint32_t base = (uint32_t) &tss_entry;
uint32_t limit = sizeof(tss_entry)-1;
 
/* Define a TSS descriptor in the GDT */
g->limit_low = limit;
g->base_low = base;
g->accessed = 1;                // if <code_data_segment>=0 (system entry), <0>=LDT <1>=TSS
g->read_write = 0;              // for a TSS, <0>=not-busy <1>=busy
g->conforming_expand_down = 0;  // TSS reserved 0
g->code = 1;                    // for a TSS, <0>=16-bit <1>=32-bit
g->code_data_segment=0;         // <0>=system-entry <1>=code-or-data-entry
g->DPL = 0;                     // ring 0
g->present = 1;
g->limit_high = (limit & (0xf << 16)) >> 16;    // mask top nibble
g->available = 0;
g->long_mode = 0;
g->big = 0;                     // reserved 0
g->gran = 0;                    // limit is in bytes, not pages
g->base_high = (base & (0xff << 24)) >> 24;     // mask top byte
```

<br> We should also initiate the TSS with zeros and set the `esp0` and `ss0` fields:

```c
memset(&tss_entry, 0, sizeof(tss_entry));
tss_entry.esp0 = KERNEL_STACK_ADDRESS;
tss_entry.ss0  = KERNEL_DATA_SEGMENT;
```

<br> Following is the TSS structure:

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
} __attribute__ ((packed)) tss_entry;
```

<br> And lastly, a TSS flush (load) function:

```gas
flush_tss:
	movw $TSS_SEG_SELCTOR ,%ax
	ltr %ax
	ret
```


### Loading a Process

The general steps for loading a process include:

* Creating a new page directory
* Copying the higher half kernel's page tables into the new page directory
* Loading and parsing the process's ELF data from disk into its address space
* Setting up scheduler data and queueing the process in the scheduler
* Returning to caller


\<It's not that compilcated, just need to describe how to do it without blocking the entire computer when loading a large program\>

---
***pass ktiva***

When a thread invokes a syscall for creating a new thread, the kernel handler should merge itself to that thread. That way, if a thread takes a lot of time to load it won't block the entire system for a few seconds, but rather, only execute the loader function on that thread time slice.

| Thread                                           | Action                        |
|--------------------------------------------------|-------------------------------|
| user thread 01                                   | clone syscall                 |
| kernel loader function thread                    | merge user and kernel threads |
| user thread 01 (in kernel loader function)       | task switch                   |
| user thread 15                                   | task switch                   |
| user thread 38                                   | task switch                   |
| user thread 01 (in kernel loader function)       | task switch                   |
| user thread 38                                   | task switch                   |
| user thread 01 (in user space)                   | task switch                   |
| user thread 05 (new thread created by thread 01) | ...                           |

Alternatively to that, I could also create a kernel thread for that while blocking the calling processes (removing it from scheduler queue)

When I think about it this behavior should be default. After the syscall the EIP will reside in the kernel space and that is all there is to it. Later it will just return to that kernel thread.


### The Initial Process
### Data Structures
### Scheduling
### The User Interface


<br>
<br>
<br>
**XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX**


# Resources

### Task State Segment (TSS)

###### Setting up a TSS
* https://wiki.osdev.org/GDT_Tutorial#What_to_Put_In_a_GDT
* https://wiki.osdev.org/Getting_to_Ring_3
* https://forum.osdev.org/viewtopic.php?t=13678

###### TSS structure and flags
* https://wiki.osdev.org/Task_State_Segment#Protected_Mode
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_01.htm
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_02.htm
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_03.htm
* https://stackoverflow.com/questions/54876039/creating-a-proper-task-state-segment-tss-structure-with-and-without-an-io-bitm

###### TSS workings
* https://stackoverflow.com/questions/68946642/x86-hardware-software-tss-usage


### Loading a Process

* https://wiki.osdev.org/Loading_a_Process
* https://forum.osdev.org/viewtopic.php?f=1&t=15622


### The Initial Process

* https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread


### Data Structures

* https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread
* https://stackoverflow.com/questions/9305992/if-threads-share-the-same-pid-how-can-they-be-identified


### Scheduling


### The User Interface

#### Other
https://wiki.osdev.org/Getting_to_Ring_3
https://wiki.osdev.org/Kernel_Multitasking
https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
https://wiki.osdev.org/Context_Switching
https://wiki.osdev.org/Scheduling_Algorithms


---

<!--
Upon entry into the kernel, the kernel stack for the thread is loaded and the user stack, along with its execution state, is saved. Each thread may have its own stack or share a set of stacks. 


Multiple Kernel Stacks
The main advantage of each thread having its own kernel stack is that system calls can block inside of the kernel and easily resume later from that point. If a page fault or interrupt were to occur during a system call, then it's possible to switch to another context and back, and later complete the system call. However, a large number of threads could tie up a significant amount of memory, most of which would sit unused at any given time. Further, the constant switching of kernel threads may lead to higher cache misses and thus poorer performance. -->


https://wiki.osdev.org/Blocking_Process
https://wiki.osdev.org/Category:Processes_and_Threads
"The initial thread" https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/aixprggd/genprogc/understanding_threads.htm


# DATA STRUCTURES
**TID** = Thread ID
**PID** = Process ID = **TGID** = Thread Group ID

- process/thread data structure