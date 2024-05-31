// File System Header File // ~ eylon

#if !defined(FS_H)
#define FS_H

#include <drivers/pata.h>
#include <libc/stdint.h>

// Disk sector index from which the file system begins
#define FS_START_SECTOR 64
// Max name length of an Inode
#define FS_MAX_NAME_LEN 128
// Number of direct refrences in an Inode
#define FS_DIRECT_NUM   12
// Number of indirect refrences in an Inode
#define FS_INDIRECT_NUM 4
// File system's magic number
#define FS_MAGIC        0xf604c7bc
// File system's path spiltting character
#define FS_SPLIT_CHAR   "/"
// Index of the inode of the root directory
#define FS_ROOT_INDEX   0

// Superblock structure
typedef struct
{
    uint32_t block_count;           // total number of blocks in the file system
    uint32_t block_size;            // block size; must be a power of 2 and greater than 512
    uint32_t inode_blocks_count;    // total number of inode blocks; total_inodes = indoes_per_block * inode_block_count
    uint32_t inodes_per_block;      // number of inodes in a single block; must be a power of 2 and greater than 0
    uint32_t super_index;           // superblock block number - starting block index 
    uint32_t fs_magic;              // file system unique magic number
} __attribute__((packed)) superblock_t;

typedef struct
{
    superblock_t sb;
    void (*read)();
    void (*write)();
} fs_t;

// Inode types
typedef uint32_t inode_type_t;
#define FS_NT_FILE  0xF     // file
#define FS_NT_DIR   0xD     // directory
#define FS_NT_LINK  0xE     // link

// Inode structure
typedef struct {
    char name[FS_MAX_NAME_LEN];         // file name
    inode_type_t type;                  // file type
    size_t count;                       // count of <NT_FILE> bytes of data/<NT_DIR> number of files in direcotry
    size_t direct[FS_DIRECT_NUM];       // direct refrence to <NT_FILE> blocks of data/<NT_DIR> inodes
    size_t indirect[FS_INDIRECT_NUM];   // block indexs containing a list to <NT_FILE> blocks of data/<NT_DIR> inodes
} __attribute__((packed)) inode_t;

// File descriptor structure
typedef struct {
    int fileno;             // file descriptor number
    uint32_t offset;        // the current position within the file
    uint32_t flags;         // flags which have been used to open the file
    inode_t inode;          // the inode of the file

    struct fd_t* next;
    struct fd_t* prev;
} fd_t;

void init_fs();

#endif