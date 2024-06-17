// sys_open // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>

/* Open and possibly create a file.
    
    The open() system call opens the file specified by pathname and returns
    a file descriptor for that file. If the specified file does not exist, it
    may optionally (if O_CREAT is specified in flags) be created by open().

    The return value of open() is a file descriptor - a small, nonnegative integer
    that is an index to an entry in the process's table of open file descriptors. 
    The file descriptor is used in subsequent system calls (read, write, lseek, and
    more) to refer to the open file.

    The argument [flags] must include one of the following access modes: O_RDONLY,
    O_WRONLY, or O_RDWR. These request opening the file read-only, write-only, or
    read/write, respectively.

    In addition, zero or more file creation flags and file status flags can be bitwise
    ORed in flags.

    int syscall(SYS_open, char* pathname, int flags);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_open(int_frame_t* param)
{
    param->eax = fs_open((char*) param->ebx, param->ecx);
}