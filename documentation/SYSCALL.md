# System Calls

System calls are used to call a kernel service from user land; they allow the user to ask the kernel for system resources (memory, IO access, file data) and to perform high priority operations (create a new process, sleep, restart).

The calls are usually being invoked by the user by raising an interrupt. The interrupt will cause a jump to the kernel that will handle the user's request before returning back to it.

### Syscall Handler

`INT 0x5C` (**S**ys**C**all) will invoke the kernel's syscall handler.