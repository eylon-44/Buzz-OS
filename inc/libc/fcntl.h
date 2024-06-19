// fcntl.h // ~ eylon

#if !defined(__LIBC_FCNTL_H)
#define __LIBC_FCNTL_H

// File descriptor flags
#define O_RDONLY       00   // open for reading only
#define O_WRONLY       01   // open for writing only
#define O_RDWR         02   // open for reading and writing
#define O_CREAT       0100  // create file if it does not exist
#define O_EXCL        0200  // error if O_CREAT and the file exists
#define O_TRUNC      01000  // truncate size to 0
#define O_APPEND     02000  // append on each write
#define O_NONBLOCK   04000  // non-blocking mode
#define O_DIRECTORY 0200000 // must be a directory

int open(const char *pathname, int flags);
int close(int fd);
int creat(const char *path);

#endif