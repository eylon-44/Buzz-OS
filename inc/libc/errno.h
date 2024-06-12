// errno.h // ~ eylon

#if !defined(__LIBC_ERRNO_H)
#define __LIBC_ERRNO_H

/* The <errno.h> header file defines symbolic names for each of the possible error
    numbers that may appear in errno. Valid error numbers are all positive numbers. 
    
    A common mistake is to do:

    if (somecall() == -1) {
        printf("somecall() failed\n");
        if (errno == ...) { ... }
    }

    where errno no longer needs to have the value it had upon return
    from somecall() (i.e., it may have been changed by the
    printf). If the value of errno should be preserved across a
    library call, it must be saved:

    if (somecall() == -1) {
        int errsv = errno;
        printf("somecall() failed\n");
        if (errsv == ...) { ... }
    }
*/

extern int errno;

#define EPERM               1       // Operation not permitted
#define ENOENT              2       // No such file or directory
#define ESRCH               3       // No such process
#define EINTR               4       // Interrupted system call
#define EIO                 5       // Input/output error
#define ENXIO               6       // No such device or address
#define E2BIG               7       // Argument list too long
#define ENOEXEC             8       // Exec format error
#define EBADF               9       // Bad file descriptor
#define ECHILD              10      // No child processes
#define ENOMEM              12      // Cannot allocate memory
#define EACCES              13      // Permission denied
#define EFAULT              14      // Bad address
#define EEXIST              17      // File does not exist
#define ENOTDIR             20      // Not a directory
#define EISDIR              21      // Is a directory
#define EINVAL              22      // Invalid argument
#define EFBIG               27      // File too large
#define ENOSPC              28      // No space left on device
#define ESPIPE              29      // Illegal seek
#define EROFS               30      // Read-only file system
#define EMLINK              31      // Too many links
#define ENAMETOOLONG        36      // File name too long
#define ENOTEMPTY           39      // Directory not empty
#define ENODATA             61      // No data available
#define EOVERFLOW           75      // Value too large for defined data type
#define EBADFD              77      // File descriptor in bad state
#define EMSGSIZE            90      // Message too long

#endif