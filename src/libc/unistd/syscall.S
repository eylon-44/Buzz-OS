// unistd/syscall.S // ~ eylon

#include <sys/syscall.h>

.section .text
.global syscall

/*  Invoke a syscall.

    syscall() invokes the system call whose assembly language interface
    has the specified number with the specified arguments.

    syscall() saves CPU registers before making the system call,
    restores the registers upon return from the system call, and
    stores any error returned by the system call in errno(3).

    Symbolic constants for system call numbers can be found in the
    header file <sys/syscall.h>.

    #include <unistd.h>
    int syscall(int sysno, ...)
*/
syscall:
    // Prologue
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    
    movl 8(%ebp), %eax     // move the syscall number into eax
    movl 12(%ebp), %ebx    // move the first argument into ebx
    movl 16(%ebp), %ecx    // move the second argument into ecx
    movl 20(%ebp), %edx    // move the third argument into edx
    movl 24(%ebp), %esi    // move the fourth argument into esi
    movl 28(%ebp), %edi    // move the fifth argument into edi

    int $SYS_int           // perform the syscall

    // Epilogue
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    movl %ebp, %esp
    popl %ebp
    ret