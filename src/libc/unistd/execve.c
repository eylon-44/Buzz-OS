// unistd/execve.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

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

    #include <unistd.h>
    int execve(const char* pathname, char* const argv[]);
*/
int execve(const char* pathname, char* const argv[])
{
    return syscall(SYS_execve, pathname, argv);
}