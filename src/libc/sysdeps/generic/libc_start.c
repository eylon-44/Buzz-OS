// sys/generic/libc_start.c // ~ eylon

#include <sysdeps/generic/crt.h>
#include <stdlib.h>

/* Start the main function. 

    The __libc_start_main() function initiates LIBC and the execution environment, calls
    the program's [main] function, and terminates the process after [main] has finished executing.

    __libc_start_main() is a private function that does not return and should only be called
    by the _start function.

    extern __libc_start_main;
    [no-return] void __libc_start_main(int argc, char* argv[], int (*main)(int argc, char* argv[]));
*/
void __libc_start_main(int argc, char* argv[], int (*main)(int, char*[]))
{
    int status;

    __libc_init();
    status = main(argc, argv);
    exit(status);
}