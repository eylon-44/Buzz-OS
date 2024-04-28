# ELF
https://wiki.osdev.org/ELF
https://wiki.osdev.org/ELF_Tutorial
https://docs.oracle.com/cd/E19455-01/806-3773/elf-2/index.html

# The Init Process
https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread
https://en.wikipedia.org/wiki/Init
http://213.254.12.151/~rubini/docs/init/
https://en.wikipedia.org/wiki/Booting_process_of_Linux#Init_process

# Keyboard
https://wiki.osdev.org/PS/2_Keyboard
https://wiki.osdev.org/%228042%22_PS/2_Controller
https://wiki.osdev.org/PS/2_Keyboard#Scan_Code_Set_1

# Locks
https://github.com/christianb93/ctOS/blob/master/doc/system/Locks.md
https://stackoverflow.com/questions/3339141/x86-lock-question-on-multi-core-cpus/3339380#3339380
https://stackoverflow.com/questions/8891067/what-does-the-lock-instruction-mean-in-x86-assembly
https://wiki.osdev.org/Spinlock

# Process Manager
To be precise, the software generated interrupt is a "trap" and is not handled the same as a hardware "interrupt". 
See https://stackoverflow.com/questions/3149175/what-is-the-difference-between-trap-and-interrupt

SIGNAL: https://www.quora.com/What-is-the-difference-between-kill-and-signal-system-calls-in-Linux

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
* https://lwn.net/Articles/631631/
* https://man7.org/linux/man-pages/man2/execve.2.html

### The Init Process
* https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread
* https://en.wikipedia.org/wiki/Init
* http://213.254.12.151/~rubini/docs/init/


### Data Structures
* https://stackoverflow.com/questions/4894609/does-a-cpu-process-always-have-at-least-one-thread
* https://stackoverflow.com/questions/9305992/if-threads-share-the-same-pid-how-can-they-be-identified
* https://forum.osdev.org/viewtopic.php?f=1&t=24452


### Scheduling
* https://en.wikipedia.org/wiki/Daemon_(computing)
* https://en.wikipedia.org/wiki/Booting_process_of_Linux#:~:text=Init%20process,-Once%20the%20kernel&text=The%20init%20system%20is%20the,was%20just%20called%20%22init%22.
* https://wiki.osdev.org/User:Mariuszp/Scheduler_Tutorial
* https://wiki.osdev.org/Scheduling_Algorithms


### The User Interface
* https://linasm.sourceforge.net/docs/syscalls/process.php
* https://filippo.io/linux-syscall-table/
* https://linux.die.net/man/2/clone
* https://lwn.net/Articles/631631/
* https://man7.org/linux/man-pages/man2/execve.2.html
* https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/

### Other
https://wiki.osdev.org/Getting_to_Ring_3
https://wiki.osdev.org/Kernel_Multitasking
https://wiki.osdev.org/Brendan%27s_Multi-tasking_Tutorial
https://wiki.osdev.org/Context_Switching
https://wiki.osdev.org/Scheduling_Algorithms
https://wiki.osdev.org/Blocking_Process
https://wiki.osdev.org/Category:Processes_and_Threads
"The initial thread" https://sites.ualberta.ca/dept/chemeng/AIX-43/share/man/info/C/a_doc_lib/aixprggd/genprogc/understanding_threads.htm


<!--
Upon entry into the kernel, the kernel stack for the thread is loaded and the user stack, along with its execution state, is saved. Each thread may have its own stack or share a set of stacks.
The main advantage of each thread having its own kernel stack is that system calls can block inside of the kernel and easily resume later from that point. If a page fault or interrupt were to occur during a system call, then it's possible to switch to another context and back, and later complete the system call. However, a large number of threads could tie up a significant amount of memory, most of which would sit unused at any given time. Further, the constant switching of kernel threads may lead to higher cache misses and thus poorer performance. -->