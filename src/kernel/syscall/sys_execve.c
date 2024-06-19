// sys_execve // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>

/* Execute a program.

    execve() executes the program referred to by [pathname]. Unlike the UNIX
    implementation, in Buzz, execve() creates a new child process for the calling
    parent and DOES NOT KILL NOR REPLACE IT.

    [pathname] must be an ELF file.

    [argv] is an array of pointers to strings passed to the new program as its
    command-line arguments. By convention, the first of these strings (argv[0])
    should contain the filename associated with the file being executed. The [argv]
    array must be terminated by a NULL pointer. (Thus, in the new program, argv[argc]
    will be NULL.)
     
    The argument vector can be accessed by the new program's main function, when it is defined as:

        int main(int argc, char *argv[]);

    On success, execve() returns the new process's PID, and on failure -1.

    int syscall(SYS_execve, const char* pathname, char* const argv[]);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_execve(int_frame_t* param)
{
    process_t* proc = pm_load(pm_get_active(), (const char*) param->ebx, (char* const*) param->ecx, PM_DEFAULT_PRIORITY);
    if (proc != NULL) {
        param->eax = proc->pid;
    }
    else {
        param->eax = -1;
    }
}
