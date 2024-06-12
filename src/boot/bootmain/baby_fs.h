// Baby File System Header File // ~ eylon

#if !defined(BABY_FS_H)
#define BABY_FS_H

#include <kernel/fs.h>

// Base of 4KB safe scratch space
#define SCRATCH_SPACE 0x10000
// Page size
#define PAGE_SIZE 0x1000

ssize_t baby_fs_read(int inode_index, void* buff, size_t count, size_t offset);
int baby_fs_seek(char* path);
void init_baby_fs();

#endif