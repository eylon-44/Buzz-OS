// unistd/lseek.c // ~ eylon

#include <unistd.h>
#include <sys/syscall.h>

/* Offset an open file.
    
    sys_lseek() repositions the file offset of the open file description
    associated with the file descriptor [fd] to the argument [offset]
    according to the directive [whence] as follows:
        SEEK_SET
            The file offset is set to offset bytes.
       SEEK_CUR
            The file offset is set to its current location plus offset bytes.
       SEEK_END
            The file offset is set to the size of the file plus offset bytes.

    Upon successful completion, sys_lseek() returns the resulting offset location
    as measured in bytes from the beginning of the file. On error, -1 is returned.

    #include <unistd.h>
    int lseek(int fd, int offset, int whence);
*/
int lseek(int fd, int offset, int whence)
{
    return syscall(SYS_lseek, fd, offset, whence);
}