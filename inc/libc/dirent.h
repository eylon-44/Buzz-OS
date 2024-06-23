// dirent.h // ~ eylon

#if !defined(__LIBC_DIRENT_H)
#define __LIBC_DIRENT_H

// https://pubs.opengroup.org/onlinepubs/7908799/xsh/dirent.h.html
// https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program

#include <limits.h>

// Directory entry types
#define DT_REG 0xF1     // regualr file
#define DT_DIR 0xD1     // directory

struct dirent
{
    unsigned char d_type;
    char d_name[FNAME_MAX];
};

#endif