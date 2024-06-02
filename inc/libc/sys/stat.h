// sys/stat.h // ~ eylon

#if !defined(__LIBC_STAT_H)
#define __LIBC_STAT_H

struct stat {
    size_t st_size;    // size of file in bytes
};

#endif