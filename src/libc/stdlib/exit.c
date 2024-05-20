// stdlib/exit.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>
#include <sysdeps/generic/crt.h>

/*  Normal process termination.

    The exit() function causes normal process termination. All functions registered with atexit() 
    are called, in the reverse order of their registration. (It is possible for one of these 
    functions to use atexit() to register an additional function to be executed during exit 
    processing; the new registration is added to the front of the list of functions that remain
    to be called.) If one of these functions does not return, then none of the remaining functions
    is called, and further exit processing (in particular, flushing of stdio streams) is abandoned.
    If a function has been registered multiple times using atexit(), then it is called as many
    times as it was registered. All open stdio streams are flushed and closed.
    
    The exit() function does not return to the current process, but does for the parent; the
    least significant byte of [status] ([status] & 0xFF) is returned to the parent.

    #include <stdlib.h>
    [no-return] void exit(int status);
*/
void exit(int status)
{
    __libc_fini();
    syscall(SYS_exit, status);
}