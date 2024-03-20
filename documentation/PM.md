# Process Manager

### Goals of a Process Manager

* Spawning and killing a process
* Spawning and killing a thread for a process
* Jumping between rings 0 and 3
* Scheduling thread execution


### Programs, Processes & Threads

A **program** is a set of instructions that a computer can execute. When we write a C or an Assembly program and compile it, the compiler produces binary code. This compiled binary code is a program. Only when we execute that program, it becomes a **process**. You can think of a process as an ‘active’ entity, opposed to a program which is a ‘passive’ entity. A **thread** is a single flow of control (code) within a process. It is where execution takes place. A process is essentially a group of threads that share the same resources, including address space, code, global data and open file streams, but a different stack. A process must own at least one thread; with no threads, a process won't be able to execute anything and therefore be useless. That means that process creation includes the act of creating at least one thread for that process. By having multiple threads, a process can execute multiple tasks concurrently.


### Context Switching & The Task State Segment

A **context** describes all the attributes that may differ in one running process from another. These attributes may include a virtual address space, common registers (eax, eip, esp, eflgas...) and a kernel stack. In other words, a context describes a thread's execution environment.

As all code execution (in this OS) takes place on a single processor, there could only be one thread runnnig at any given moment. In order for the operating system to achieve a multitasking effect, we can execute many rapid **context switches** between all threads, and by that give each thread its own time splice for executing.

The Task State Segment (TSS) is a data structure originally designed to be used with hardware task-switching, where each individual task has its own TSS. Because software task-switching it faster and more flexable, the TSS is rarely being used for its original purpose. 

The TSS initiation process includes creating a segment for it in the GDT in the following manner:

```c
// create a gdt entry
#define GDT_ENTRY(base, limit, dpl, access_options, flags_options) { ... }

// tss segment
GDT_ENTRY(&TSS, sizeof(TSS)-1, 0,
        GDT_ACCESS_ACCESSED | GDT_ACCESS_PRESENT | GDT_ACCESS_CODE_SEG,     // 0x89
        0)
```

The TSS structure is as follows:

```c
typedef struct {
	uint32_t prev_tss;  // The previous TSS - with hardware task switching these form a kind of backward linked list.
	uint32_t esp0;      // The stack pointer to load when changing to kernel mode.
	uint32_t ss0;       // The stack segment to load when changing to kernel mode.
	// Everything below here is unused.
	uint32_t esp1;      // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__ ((packed)) tss_entry;
```

# TSS FLUSH = TSS LOAD
## I don't know why people call it that way

#### TSS

Setting up a TSS
* https://wiki.osdev.org/GDT_Tutorial#What_to_Put_In_a_GDT
* https://wiki.osdev.org/Getting_to_Ring_3
* https://forum.osdev.org/viewtopic.php?t=13678

TSS structure and flags
* https://wiki.osdev.org/Task_State_Segment#Protected_Mode
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_01.htm
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_02.htm
* https://pdos.csail.mit.edu/6.828/2018/readings/i386/s07_03.htm
* https://stackoverflow.com/questions/54876039/creating-a-proper-task-state-segment-tss-structure-with-and-without-an-io-bitm

TSS workings
* https://stackoverflow.com/questions/68946642/x86-hardware-software-tss-usage

#### Other
https://wiki.osdev.org/Kernel_Multitasking
https://forum.osdev.org/viewtopic.php?f=1&t=15622
https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
https://wiki.osdev.org/Loading_a_Process
https://wiki.osdev.org/Context_Switching
https://wiki.osdev.org/Scheduling_Algorithms
https://forum.osdev.org/viewtopic.php?f=1&t=21251

"You only need one TSS per processor. It's mainly used for the stack pointers upon privilege level switches and the I/O permission bitmap. It used to be used for hardware task switching, but it's no longer supported in 64-bit mode. Even 32-bit OSes opted for software task switching instead because it's faster."

"If you are running everything in Ring 0 then you don't need a TSS as there are no ring transitions that will occur. If however you have code running in Ring 3 (What most people call user mode), you will need a TSS to transition from Ring 3 to Ring 0 on an interrupt. When everything is in Ring 0 when an interrupt occurs the kernel (Ring 0) stack will be used to push interrupt data."

<!--
Upon entry into the kernel, the kernel stack for the thread is loaded and the user stack, along with its execution state, is saved. Each thread may have its own stack or share a set of stacks. 


Multiple Kernel Stacks
The main advantage of each thread having its own kernel stack is that system calls can block inside of the kernel and easily resume later from that point. If a page fault or interrupt were to occur during a system call, then it's possible to switch to another context and back, and later complete the system call. However, a large number of threads could tie up a significant amount of memory, most of which would sit unused at any given time. Further, the constant switching of kernel threads may lead to higher cache misses and thus poorer performance. -->

---
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
| user thread 01 (in user space with new thread)   | task switch                   |
| user thread 05 (new thread created by thread 01) | ...                           |
---


```c
typedef struct {
    uint16_t   link;
    uint16_t   link_h;

    uint32_t   esp0;
    uint16_t   ss0;
    uint16_t   ss0_h;

    uint32_t   esp1;
    uint16_t   ss1;
    uint16_t   ss1_h;

    uint32_t   esp2;
    uint16_t   ss2;
    uint16_t   ss2_h;

    uint32_t   cr3;
    uint32_t   eip;
    uint32_t   eflags;

    uint32_t   eax;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t    ebx;

    uint32_t   esp;
    uint32_t   ebp;

    uint32_t   esi;
    uint32_t   edi;

    uint16_t   es;
    uint16_t   es_h;

    uint16_t   cs;
    uint16_t   cs_h;

    uint16_t   ss;
    uint16_t   ss_h;

    uint16_t   ds;
    uint16_t   ds_h;

    uint16_t   fs;
    uint16_t   fs_h;

    uint16_t   gs;
    uint16_t   gs_h;

    uint16_t   ldt;
    uint16_t   ldt_h;

    uint16_t   trap;
    uint16_t   iomap;

} tss_struct;
```

### Data Structures #prototype

**TID** = Thread ID
**PID** = Process ID = **TGID** = Thread Group ID


```
6.1.2. Task State
The following items define the state of the currently executing task:
The task’s current execution space, defined by the segment selectors in the segment
registers (CS, DS, SS, ES, FS, and GS).
The state of the general-purpose registers.
The state of the EFLAGS register.
The state of the EIP register.
The state of control register CR3.
The state of the task register.
The state of the LDTR register.
The I/O map base address and I/O map (contained in the TSS).
Stack pointers to the privilege 0, 1, and 2 stacks (contained in the TSS).
Link to previously executed task (contained in the TSS).
```


### Loading a Process

* Create a new page directory
* Copy kernel page tables into the new page directory
* Load the process
* Return to caller

---
What about TSS?
https://forum.osdev.org/viewtopic.php?p=112592#p112592
https://wiki.osdev.org/Blocking_Process
https://wiki.osdev.org/Category:Processes_and_Threads
**REALL GOOD ANSWER** https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread
**THE GRAPHICS** https://stackoverflow.com/questions/9305992/if-threads-share-the-same-pid-how-can-they-be-identified
"The initial thread" https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/aixprggd/genprogc/understanding_threads.htm

---

### Constructing a Process Data Structure




### Common Process Management System Calls
* **fork()** Creates a new process by duplicating the calling process. The new process, called the child process, is an exact copy of the calling process, called the parent process.

* **exec()** Replaces the current process with a new one. It loads a new program into the current process's memory space and begins executing it effectively changing the program executed by the process.

* **wait()** Suspends execution of the calling process until one of its child processes terminates. It allows the parent process to synchronize its execution with the termination of a child process.

* **exit()** Terminates the calling process and returns an exit status to the operating system. The exit status typically indicates the success or failure of the process.

* **waitpid()** Similar to wait(), but allows the parent process to specify which child process it is waiting for by its process ID (PID). This enables more precise control over process synchronization.

* **kill()** Sends a signal to a specified process or group of processes. Signals are used for inter-process communication and can be used to request termination, handle errors, or trigger specific behavior in the receiving process.

* **getpid()** Returns the process ID (PID) of the calling process. This ID uniquely identifies the process within the operating system.

* **getppid()** Returns the process ID (PID) of the parent process of the calling process. Useful for determining the parent-child relationship between processes.

* **nice()** Adjusts the scheduling priority of a process. A higher priority value makes the process less likely to be scheduled for execution by the operating system, while a lower priority value increases the likelihood of execution.