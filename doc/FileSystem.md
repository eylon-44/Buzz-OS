# File System

File systems are the operating system's method of ordering data on persistent storage devices like disks. They provide an abstracted interface to access data on these devices in such a way that it can be read or modified efficiently.

This file system is a hierarchical file system, meaning that it stores file and directories in a tree-like structure. It uses parent-child relationships, where each directory can contain subdirectories and files. The top-level directory is known as the root directory, and all other directories stem from it.

In order to do so we will have to devide the linear continuous disk into manageable chunks of data, keep track of which of these chunks are free to use, and which of these chunks contain the data of a certain file.

We devide the disk with **Blocks**, keep track of the Blocks with **Mapblocks**, and keep information about a file or directory in an **Inode**.


## What is a Block
The file system divides up disk space into logical blocks of contiguous space. The size of each block is defined in `FS_BLOCK_SIZE` and may not be the same as the sector size of the disk. Blocks may contain Mapblocks, file data, or Inodes.


## What is a Mapblock
A Mapblock is a single block describing the status of the Blocks that come after it. Each bit in the Mapblock represents the status of a certain Block. Blocks may be free (0) or used (1).

For the sake of the example, let's assume that `FS_BLOCK_SIZE` is equal to 1 byte - 8 bits.

| Base Address | Type     |
|--------------|----------|
| 00           | Mapblock |
| 08           | Block    |
| 16           | Block    |
| 24           | Block    |
| 32           | Block    |
| 40           | Block    |
| 48           | Block    |
| 56           | Block    |
| 64           | Block    |
| 72           | Mapblock |
| 80           | Block    |
| 88           | Block    |
| ...          |          |

In this case, the first bit in the first Mapblock would represent the Block at address `08`, the second bit the Block at address `16` and so on. The first bit in the second Mapblock would represent the Block at address `80`, the second bit the Block at address `88` and so on.


## What is an Inode
An Inode is a structure that represents a file, directory or a symbolic link. Inodes only contain meta data that links to the blocks that actually contain the data. A single Inode takes a single Block. The start of the Inode Block contains attributes about the Inode itself and the rest of the Block is a list of Block indexes to which the Inode points to. The list is arranged in the order of the data it points to and terminates with a NULL pointer.

```c
typedef enum
{
    NT_FILE,
    NT_DIR,
    NT_LINK
} node_type_t;

typedef struct
{
    char[FS_NAME_LEN] name;
    node_type_t type;
    int open;
    size_t* blocks;
} inode_t;
```