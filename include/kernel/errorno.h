// Error Numbers Header File // ~ eylon

#if !defined(ERRORNO_H)
#define ERRORNO_H

#define ENOMEM          101     // Out of memory error
#define EPERM           105     // Operation not permitted
#define EAGAIN          106     // Resource temporarily unavailable
#define EINVAL          107     // Invalid argument
#define ENOSYS          108     // Function not implemented
#define EALREADY        109     // Operation already in progress
#define ENODEV          110     // No such device
#define EIO             111     // Input/output error
#define EBUSY           112     // Device or resource busy
#define ENOTDIR         113     // Not a directory
#define ENOEXEC         114     // Exec format error
#define EBADF           115     // Bad file descriptor
#define ENOENT          116     // No such file or directory
#define ENOSPC          117     // No space left on device
#define E2BIG           118     // Argument list too long
#define ERANGE          119     // Result too large
#define ECHILD          120     // No child processes
#define ESRCH           121     // No such process
#define EPAUSE          122     // Interrupted system call
#define EINTR           004     // Interrupted system call
#define EACCES          124     // Permission denied
#define EMFILE          125     // Too many open files
#define EPIPE           126     // Broken pipe
#define ENFILE          127     // Too many open files in system
#define ESPIPE          128     // Invalid seek
#define EWOULDBLOCK     129     // Operation would block (may be same as EAGAIN)
#define EEXIST          130     // File exists
#define ENOTTY          131     // Not a typewriter
#define EOVERFLOW       132     // Value too large for defined data type
#define EISDIR          133     // Is a directory
#define ENETUNREACH     134     // Network is unreachable
#define EADDRINUSE      135     // Address already in use
#define ENOTCONN        136     // Transport endpoint is not connected
#define ETIMEDOUT       137     // Connection timed out
#define ENOTSOCK        138     // Socket operation on non-socket
#define EAFNOSUPPORT    139     // Address family not supported by protocol
#define EADDRNOTAVAIL   140     // Cannot assign requested address
#define ECONNRESET      141     // Connection reset by peer
#define ECONNREFUSED    142     // Connection refused
#define EMSGSIZE        143     // Message too long
#define ENOBUFS         144     // No buffer space available
#define EISCONN         145     // Socket is already connected
#define EDOM            146     // Mathematics argument out of domain of function
#define EILSEQ          147     // Illegal byte sequence
#define EXDEV           148     // Cross-device link
#define EMLINK          149     // Too many links
#define EFAULT          150     // Bad address
#define EOPNOTSUPP      10045   // Operation not supported on socket
#define ELOOP           10062   // Too many levels of symbolic links
#define ENAMETOOLONG    036     // File name too long
#define EROFS           151     // Read-only file system
#define ENOTEMPTY       152     // Directory not empty

#endif