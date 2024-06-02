// unnistd.h // ~ eylon

#if !defined(__LIBC_UNISTD_H)
#define __LIBC_UNISTD_H

/* Seek Options */
#define SEEK_SET 0      // the file offset is set to offset bytes
#define SEEK_CUR 1      // the file offset is set to its current location plus offset bytes
#define SEEK_END 2      // the file offset is set to the size of the file plus offset bytes

int syscall(int sysno, ...);

#endif