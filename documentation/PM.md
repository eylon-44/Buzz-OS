# Process Manager

### Goals of a Process Manager

* Spawning and killing a process
* Spawning and killing a new thread for a process
* Scheduling thread execution and context switching


### Programs, Processes & Threads

A **program** is a set of instructions that a computer can execute. When we write a C or Assembly program and compile it, the compiler produces binary code. This compiled binary code is a program. Only when we execute that program, it becomes a **process**. You can think of a process as an ‘active’ entity, opposed to a program which is a ‘passive’ entity. A **thread** is a single flow of control (code) within a process. It is where execution takes place. A process is essentially a group of threads that share the same resources, including address space, code, global data and open file streams, but a different stack. A process must own at least one thread; with no threads, a process won't be able to execute anything and therefore be useless. That means that process creation includes the act of creating at least one thread for that process. By having multiple threads, a process can execute multiple tasks concurrently.

As all code execution (in this OS) takes place on a single processor, there could only be one thread runnnig at any given moment. While we can't truly execute different threads at the same time, we can create a simillar illusion by switching between them really fast using a **scheduler**. A scheduler is the component that manages thread execution order and creates the effect of multi-tasking by swiftly changing between different threads.

There are two main types of threads, **user-level threads** and **kernel-level threads**. User-level threads are managed by a user-level scheduler, that is, a scheduler included in the user's compiled program that manages the program's different streams of execution. Kernel-level threads are managed by a kernel-level scheduler, [...]


### Context Switching & The TSS





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