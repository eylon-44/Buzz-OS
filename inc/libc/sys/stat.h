// sys/stat.h // ~ eylon

#if !defined(__LIBC_STAT_H)
#define __LIBC_STAT_H

#include <stddef.h>
#include <dirent.h>

struct stat {
    size_t size;        // size of file in bytes, or number of files in directory
    unsigned char type; // file type
    int indx;           // inode index
    int exe;            // <0>=not-executable <1>=executable
};

int stat(const char* pathname, struct stat* statbuf);
int fstat(int fd, struct stat* statbuf);
int mkdir(const char* pathname);

#endif