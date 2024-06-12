# File System
The Buzz OS file system is a hierarchical file system, meaning that it stores files and directories in a tree-like structure by using parent-child relationships. The top-level directory is known as the root directory, and all other directories stem from it.


## What is a Block?
The file system divides up the disk space into logical chunks of contiguous space called **blocks**. The size of each block is defined in the [superblock](#what-is-a-superblock) and may not be the same as the sector size of the disk. Blocks may contain superblocks, mapblocks, file contents, or inodes.


## What is a Superblock?
In order to use the file system, we must first find the **superblock**. The superblock contains all information needed about the layout of the file system. The physical disk location of the superblock may vary; regardless, this location marks the start of the file system.

```c
typedef struct
{
    uint32_t block_count;           // total number of blocks in the file system
    uint32_t block_size;            // block size; must be a power of 2 and greater than 512
    uint32_t inode_blocks_count;    // total number of inode blocks; total_inodes = indoes_per_block * inode_block_count
    uint32_t inodes_per_block;      // number of inodes in a single block; must be a power of 2 and greater than 0
    uint32_t super_index;           // superblock block number - starting block index 
    uint32_t fs_magic;              // file system unique magic number
} __attribute__((packed)) superblock_t;
```


## What is a Mapblock?
A **mapblock** is a block defining a bitfield that describes the status of a component in the file system. Each bit in a mapblock represents the status of a certain entity; an off-bit (0) stands for free entity while an on-bit (1) for a used one.

There are two types of mapblocks in the file system, **blockmaps** and **inodemaps**. Blockmaps keep track over block statuses; the superblock, blockmaps, inodemaps, inode blocks, and any used data block will be marked with 1 to indicate that a block is reserved for a certain purpose and should not be overriden. Inodemaps keep track of inodes availability. The need for two different maps is derived from the differences in the indexing scheme between inodes and blocks.

There are a total of `ROUND_UP((inodes_per_block*inode_blocks_count) / (block_size*8))` inodemap blocks, stacked right above `ROUND_UP(block_count / (block_size*8))` mapblocks, which are stacked right above the superblock.

Assuming the entire blockmap list is layed flat in memory, we can get the bit that represents the status of a certain block by using this expression:
```
status =
*( super_index + 1 + (block_index/(block_size*8))       // blockmap block index
* block_size                                            // blockmap base address
+ (block_index%(block_size*8)) )                        // access searched block byte address
& 1 << block_index%8                                    // searched block bit
```
A similar principle may be applied for inodemaps.

Each time a block a or an inode is being allocated or deallocated, we must update the mapblocks accordingly.


## What is an Inode?
An **inode** is a structure that represents a file, directory or a symbolic link. Inodes contain meta data that links to the blocks that contain the actual data.

There may be multiple inodes in a single block, therefore we cannot refrence an inode the same way we refrence a block; inodes must have their own indexing scheme. The first inode block is located right above the inodemaps; we can get its block index by using this expression: `super_index + 1 + number_of_mapblocks`. We can now get the offset of the inode within its block with this expression: `block_size/(inode_index % inodes_per_block)`.

The inode data structure contains a `direct` list of block indexes. If the inode describes a file, the list defines indexes of blocks that contain the actual data of the file. If the inode describes a directory, the list defines indexes of inodes that belong to this directory.

```c
typedef uint32_t inode_type_t;
#define FS_NT_FILE  0xF     // file
#define FS_NT_DIR   0xD     // directory
#define FS_NT_LINK  0xE     // link

typedef struct {
    char name[FS_MAX_NAME_LEN];         // file name
    inode_type_t type;                  // file type
    size_t count;                       // count of <NT_FILE> bytes of data/<NT_DIR> number of files in direcotry
    size_t direct[FS_DIRECT_NUM];       // direct refrence to <NT_FILE> blocks of data/<NT_DIR> inodes
} __attribute__((packed)) inode_t;
```

## File System Disk Layout
Following is a general layout of the disk including all of the components described above:
```
   ┌── ┌── ┌────...N...────┐
   │   │   │               │
Sector │   │               │
   │ block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Data blocks   │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ inodes        │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ inodemaps     │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ blockmaps     │
   └── └── │               │
   ┌── ┌── ├───────────────┤
   │   │   │               │
Sector │   │               │
   │ block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ superblock    │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │ Bootloader    │
   │ block │               │
   ├── │   ├───────────────┤
   │   │   │               │
Sector │   │ MBR           │
   │   │   │               │
   └── └── └───────────────┘
```

## File Management
### File Seeking
In order to perform any operation on a certain file, we must first find it.

The first step in the process involves splitting the file's path by removing all `/` in it (`/path/to/file` → `["path", "to", "file"]`). Next, we are going to step each element in the splited path. If the file exists, each file in the list except the last one should point to the one that comes right after it. Mind that, while not appearing in the list, `/` is our root directory - the top-most directory. Because all other directories derive from it, our search will always start from there. The inode of the root directory is the first inode - inode 0 - so we can always find it easily.

Let's try to apply this on `/games/pacman.elf`, assuming our file system is constructed as follows:
```
/
├── apps
│   ├── compiler.elf
│   └── text-editor.elf
├── code
│   ├── donkey-kong.c
│   └── mario
│       └── mario.c
├── donut.swp
└── games
    ├── pacman.elf
    └── tetris.elf

```

We start by splitting the path: `games/pacman.elf` → `["games", "pacman.elf"]`.

Next, we read inode 0 - the root directory: 
```
name     = "/";
type     = NT_DIR;
count    = 4;
direct   = [7, 0, 5, 16, 9, 0, ...];
```

`name` tells us that we are indeed in the root directory, and `type` tells us that it is a directory, as expected. Because this is a directoy, `count` defines the number of files it contains (instead of the size of the data), and the `direct` list points to inodes (instead of blocks), which are the files of the directory.

We are now going to scan the directory's `direct` list until finding a matching entry or until reading `count` entries.

The first item in the list is 7. Let's read inode 7:
```
name     = "code";
type     = NT_DIR;
count    = 2;
direct   = [32, 12, 0, ...];
```

We have found the "code" directory, which is not what we were searching for; let's continue. The next inode is inode 0 - if listed in a `direct` list, inode 0 marks an invalid inode, and therefore we ignore it and continue to the next one. The next inode is inode 5, let's read it:
```
name     = "games";
type     = NT_DIR;
count    = 2;
direct   = [24, 8, ...];
```

We have found the "games" directory. We now need to scan it and find the "pacman.elf" file in it. Same process applies. Let's read inode 24:
```
name     = "pacman.elf";
type     = NT_FILE;
count    = 32768;
direct   = [44, 5, 0, 6, 0, ...];
```

Looks like we have found our "pacman.elf" file, which is a normal file with the size of `32768` bytes.

Note that because this is a file and not a directory, the `direct` list contains block indexes instead of inode indexes.


### File Destruction
We have only now realised that our text-editor app have left us a `/donut.swp` file in our root directory. Donuts are great, but not swap files! Let's delete it.
```
/
├── apps
│   ├── compiler.elf
│   └── text-editor.elf
├── code
│   ├── donkey-kong.c
│   └── mario
│       ├── mario.c
├── donut.swp <--- DELETE
└── games
    ├── pacman.elf
    └── tetris.elf
```

Here is inode 0 again:
```
name     = "/";
type     = NT_DIR;
count    = 4;
direct   = [7, 0, 5, 16, 9, 0, ...];
```

We already know that the first and third inodes point to the `code` and `games` directories, so will start by reading the forth one - inode 16:
```
name     = "donut.swp";
type     = NT_FILE;
count    = 13673;
direct   = [92, 63, 0, ...];
```

We found a match. Deleting the file involes freeing blocks 92 and 63 (data blocks), and setting item number 4 in the root directory's `direct` list to 0.


### File Creation
We have completed writing our mario game and we would like to compile it. We send the code to the compiler to do its magic and ask it to call the new file `/code/mario/mario.elf`.

How can we create this new file?

```
/
├── apps
│   ├── compiler.elf
│   └── text-editor.elf
├── code
│   ├── donkey-kong.c
│   └── mario
│       ├── mario.c
│       └── mario.elf <--- NEW FILE
└── games
    ├── pacman.elf
    └── tetris.elf
```

As so:
* Find a free inode index in the [inodemap](#what-is-a-mapblock) and set it to used.
* Create and set an inode and place it at the index we have allocated for it.
* Seek the new file's parent inode. In our case it's the inode of the `/code/mario` directory.
* Add the index of the new file's inode into the `direct` list of the parent directory.

### File Reading & Writing
The file's data is referenced in the direct list of its inode. Finding the file's inode allows us to access its data blocks, where we can read and write file's data.

As opposed to directory inodes, the `direct` list in file inodes must be continuous. There may be no gaps of null values between block indexes.

### File Renaming
We now want to move our compiled mario game into the `games` directory while changing its name to `super-mario.elf`.
```
/
├── apps
│   ├── compiler.elf
│   └── text-editor.elf
├── code
│   ├── donkey-kong.c
│   └── mario
│       ├── mario.c
│       └──         <--- OLD LOCATION
└── games
    ├── pacman.elf
    ├── super-mario.elf <--- NEW LOCATION
    └── tetris.elf
```

Easy,
* Seek the file's inode.
* Remove it from its parent's `direct` list.
* Add it to its new parent's `direct` list.
* Change the inode's `name` attribute to the new name.


## File Descriptors
Among other things that the OS tracks for each process, file descriptors are one of them. A file descriptor (FD) is an unsigned integer provided by the kernel to the user as an identifier for an open file. When a process requests the kernel to open a file, the kernel allocates some memory to save the file's metadata and returns a unique number that references it. The file descriptor is used by the user to refer to one of its files when performing a file operation, such as read, write and close.

As mentioned above, the kernel returns a file descriptor as a non-negative value. Negative file descriptor values are invalid and therefore indicate an error.

Each process has its own file descriptors list starting from index 0. The list initiates with the three standard file descriptors, corresponding to the three standard streams:

| Value | Name              | <unistd.h> symbolic constant | <stdio.h> file stream |
|-------|-------------------|------------------------------|-----------------------|
| 0     | Standard input    | STDIN_FILENO                 | stdin                 |
| 1     | Standard output   | STDOUT_FILENO                | stdout                |
| 2     | Standard error    | STDERR_FILENO                | stderr                |

You might have noticed that the LIBC file descriptor `FILE*` you get when you open a file is not an unsigned int, but rather a struct poniter. This is because LIBC keeps some extra metadata about the opened file in the heap, but the kernel only really cares about the file descriptor number.

In the kernel, file descriptors are saved in their corresponding process structure as a linked list. The file descriptor structure is defined as follows:

```c
// File descriptor structure
typedef struct fd {
    int fileno;             // file descriptor number
    int flags;              // file descriptor flags
    uint32_t offset;        // the current position within the file
    size_t inode;           // the inode index of the file

    struct fd* next;
    struct fd* prev;
} fd_t;
```