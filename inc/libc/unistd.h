// unnistd.h // ~ eylon

#if !defined(__LIBC_UNISTD_H)
#define __LIBC_UNISTD_H

#include <stddef.h>

/* Streams */
#define STDIN_FILENO  0     // standard input
#define STDOUT_FILENO 1     // standard output
#define STDERR_FILENO 2     // standard error

/* Seek Options */
#define SEEK_SET 0      // the file offset is set to offset bytes
#define SEEK_CUR 1      // the file offset is set to its current location plus offset bytes
#define SEEK_END 2      // the file offset is set to the size of the file plus offset bytes

int syscall(int sysno, ...);
ssize_t read(int fd, void* buff, size_t count);
ssize_t write(int fd, const void* buff, size_t count);
int brk(void *addr);
void* sbrk(size_t increment);
int unlink(const char *pathname);
int execve(const char* pathname, char* const argv[]);
int milisleep(size_t miliseconds);
int lseek(int fd, int offset, int whence);
int truncate(const char *path, size_t length);
int ftruncate(int fd, size_t length);
int rmdir(const char *path);
int chdir(const char *path);
char* getcwd(char* buff, size_t size);

#endif