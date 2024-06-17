// fcntl.h // ~ eylon

#if !defined(__LIBC_FCNTL_H)
#define __LIBC_FCNTL_H

// File descriptor flags
#define O_RDONLY       00   // open for reading only */
#define O_WRONLY       01   // open for writing only */
#define O_RDWR         02   // open for reading and writing */

int open(const char *pathname, int flags);

#endif