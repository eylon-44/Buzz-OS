// sys/i386/crt0.S // ~ eylon

.section .text
.global _start
.extern main
.extern __libc_start_main

/* C Runtime 0.
    
    crt0 is a set of execution startup routines linked into a C program that performs
    any initialization work required before calling the program's main function.

    Here defined the _start function, which is the first piece of code to run in a
    program (the linker sets the ELF entry to point at _start).

    The _start function initializes the stack, calls the __libc_start_main function with
    the required parameters, and makes sure the program terminates.

    The startup code below assumes that the program loader have placed [argc] at the top
    of the stack and [argv] right above it.
*/
_start:
    xorl %ebp, %ebp         // reset EBP to mark the outermost frame
    popl %esi               // save [argc] in ESI
    movl %esp, %ecx         // save [argv] in ECX
    andl $0xfffffff0, %esp  // 16-byte align ESP
    pushl %ecx              // push [argv]
    pushl %esi              // push [argc]
    pushl main              // push the [main] function
    call __libc_start_main  // call [__libc_start_main]
    
    // If [__libc_start_main] couldn't exit for any reason, exit by rasing a GPF
    _force_exit:
        hlt
        jmp _force_exit