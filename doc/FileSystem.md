# File System
The Buzz OS file system is a hierarchical file system, meaning that it stores files and directories in a tree-like structure by using parent-child relationships. The top-level directory is known as the root directory, and all other directories stem from it.


## What is a Block?
The file system divides up the disk space into logical chunks of contiguous space called **Blocks**. The size of each block is defined in the [Superblock](#what-is-a-superblock) and may not be the same as the sector size of the disk. Blocks may contain Superblocks, Mapblocks, file content, or Inodes.


## What is a Superblock?
In order to use the file system, we must first find it's Superblock. The Superblock contains all information needed about the layout of the file system. The physical disk location of the Superblock may vary; regardless, this location marks the start of the file system.

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
A **Mapblock** is a block defining a bitfield that describes the status of a component in the file system. Each bit in a Mapblock represents the status of a certain entity; an off-bit (0) stands for free while an on-bit (1) for used.

There are two types of Mapblocks in the file system, **Blockmap** and **Inodemap**. Blockmaps keep track over Block statuses; the Superblock, Blockmaps, Inodemaps, Inode Blocks, and any used data Block will be marked with 1 to indicate that the Block is reserved for a certain purpose. Inodemaps indicate the status of Inodes. The need for two different maps is derived from the differences in the indexing scheme between Inodes and Blocks.

There are a total of `ROUND_UP((inodes_per_block*inode_blocks_count) / (block_size*8))` Inodemap Blocks, stacked right above  `ROUND_UP(block_count / (block_size*8))` Mapblocks, which are stacked right above the Superblock.

Assuming the entire Blockmap list is layed flat in memory, we can get the bit that represents the status of a certain Block by using this expression:
```
status =
*( super_index + 1 + (block_index/(block_size*8))       // blockmap block index
* block_size                                            // blockmap base address
+ (block_index%(block_size*8)) )                        // access searched block byte address
& 1 << block_index%8                                    // searched block bit
```
A similar principle applies for Inodemaps.

Each time a Block a or an Inode is being allocated or deallocated, we must update the Mapblocks.


## What is an Inode?
An Inode is a structure that represents a file, directory or a symbolic link. Inodes contain meta data that links to the Blocks that contain the actual data.

There may be multiple Inodes in a single Block, therefore we cannot refrence an Inode by using a Block index; Inodes must have their own indexing scheme. The first Inode Block is stacked right above the Inodemaps; we can get its Block index by using this expression: `super_index + 1 + number_of_mapblocks`. We can now get the offset of the Inode within its Block with this expression: `block_size/(inode_index % inodes_per_block)`.

The Inode data structure contains a `direct` and an `indirect` list of Block indexes. The `direct` list defines indexes of Blocks containing actual file data, while the `indirect` list defines indexes of Blocks containing a `direct` list inside of them. You can think of a `direct` list as a pointer to the data, and an `indirect` list as a double poniter to the data.

If `data_size > FS_DIRECT_NUM * block_size`, we access the `indirect` list.

```c
typedef enum
{
    NT_FILE = 0xF,
    NT_DIR  = 0xD,
    NT_LINK = 0xE
} inode_type_t;

typedef struct {
    char name[FS_MAX_NAME_LEN];         // file name
    inode_type_t type;                  // file type
    size_t count;                       // count of <NT_FILE> bytes of data/<NT_DIR> number of files in direcotry
    size_t direct[FS_DIRECT_NUM];       // direct refrence to <NT_FILE> blocks of data/<NT_DIR> inodes
    size_t indirect[FS_INDIRECT_NUM];   // block indexs containing a list to <NT_FILE> blocks of data/<NT_DIR> inodes
} __attribute__((packed)) inode_t;
```


## Disk Layout
Following is a general layout of the disk including all of the components described above:
```
   ┌── ┌── ┌────...N...────┐
   │   │   │               │
Sector │   │               │
   │ Block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Data Blocks   │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ Block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Inodes        │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ Block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Inodemaps     │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │               │
   │ Block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Blockmaps     │
   └── └── │               │
   ┌── ┌── ├───────────────┤
   │   │   │               │
Sector │   │               │
   │ Block │               │
   ├── │   │               │
   │   │   │               │
Sector │   │               │
   │   │   │ Superblock    │
   └── └── │               │
   ┌── ┌── ├────...N...────┤
   │   │   │               │
Sector │   │ Bootloader    │
   │ Block │               │
   ├── │   ├───────────────┤
   │   │   │               │
Sector │   │ MBR           │
   │   │   │               │
   └── └── └───────────────┘
```

## File Management
### File Seeking
In order to perform any operation on a certain file, we must first find it.

The first step in the process would be to split the file's path by removing all `/` in it: `/path/to/file` → `["path", "to", "file"]`. Next, we are going to step each element in the list; if the file exists, each element should point to the one that comes right after it. Mind that, while not appearing in the list, `/` is our root directory - the top-most directory. Because all other directories derive from the it, our search should start from there. The Inode of the root directory is the first Inode - Inode 0.

Let's try to apply that on `/games/pacman.elf`, assuming our file system looks as follows:
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

We start by reading Inode 0 - the root directory: 
```
name     = "/";
type     = NT_DIR;
count    = 4;
direct   = [7, 0, 5, 16, 9, 0, ...];
indirect = [0, ...];
```

`name` tells us that we are indeed in the root directory, and `type` tells us that Inode 0 is a directory, as expected. Because this is a directoy, `count` defines the number of files in it (instead of the size of the data), and the `direct` and `indirect` lists point to Inodes (instead of Blocks).

Let's spilt the path: `games/pacman.elf` → `["games", "pacman.elf"]`. We are now going to scan the `direct` and `indirect` lists in order until finding a matching entry or until finding `count` valid entries.

The first item in the `direct` list is 7. Let's read Inode 7:
```
name     = "code";
type     = NT_DIR;
count    = 2;
direct   = [32, 12, 0, ...];
indirect = [0, ...];
```

We have found the "code" directory, which is not what we were searching for; let's continue. The next Inode is Inode 0 - Inode 0 marks an invalid Inode; therefore we ignore it and continue to the next Inode. The next Inode is Inode 5, let's read it:
```
name     = "games";
type     = NT_DIR;
count    = 2;
direct   = [0, ...];
indirect = [24, 0, ...];
```

We have found the "games" directory; we now need to find the "pacman.elf" file in it. `count` tells us that this directory contains 2 files. Becuase the `direct` list is empty, they must be in the `indirect` list. The first item in the `indirect` list is 24, meaning that Block 24 contains an Inode refrence list. Let's read it:
```
0, 0, 8, 0, 42, 0, ...
```

We already know that zeros should be ignored, so we skip them to read Inode 8:
```
name     = "pacman.elf";
type     = NT_FILE;
count    = 32768;
direct   = [44, 5, 0, 6, 0, ...];
indirect = [23, 0, ...];
```

Looks like we have found our "pacman.elf" file, which is a normal file with the size of `32768` bytes.

Note that because this is a file and not a directory or a link, the `direct` list contains Block indexes instead of Inode indexes.


### File Destruction
We have only now realised that our text-editor have left us a `/donut.swp` file in our root directory. Donuts are great, but not swap files! Let's delete it.

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

Here is Inode 0:
```
name     = "/";
type     = NT_DIR;
count    = 4;
direct   = [7, 0, 5, 16, 9, 0, ...];
indirect = [0, ...];
```

We already know that the first and third Inodes point to the `code` and `games` directories, so will start by reading the forth one - Inode 16:
```
name     = "donut.swp";
type     = NT_FILE;
count    = 13673;
direct   = [92, 63, 0, ...];
indirect = [39, 0, ...];
```

We found a match. Deleting the file involes freeing Blocks 92 and 63 (direct Blocks), freeing the Blocks that are listed in Block 39, freeing Block 39 (indirect Blocks) and setting item number 4 in the root directory's `direct` list to 0.


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

* Find a free Inode in the [Inodemap](#what-is-a-mapblock).
* Initiate it as an empty file (size and refrence lists are zeroed out) and set its name and type.
* Seek the new file's parent Inode. In our case it's the Inode of the `/code/mario` directory.
* Add the index of the file's Inode into the refrence list of the parent directory.

### File Reading & Writing
The file's data is referenced in the direct and indirect lists of its inode. Finding the file's inode allows us to access its data blocks, where we can read and write file's data.

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

* Seek the file's Inode index.
* Remove it from its parent's refrence list.
* Set it in its new parent's refrence list.
* Change the Inode's `name` attribute to the new name.


## File Descriptors
Among other things that the OS tracks for each process, file descriptors are one of them. A file descriptor (FD) is an unsigned integer provided by the kernel to the user as an identifier for an open file. When a process requests the kernel to open a file, the kernel allocates some space to save the file's metadata and returns a unique number that references it. The file descriptor is used by the user to refer to one of its files when performing a file operation, such as read, write and close.

As mentioned above, the kernel returns a file descriptor as a non-negative value. Negative file descriptor values are invalid and therefore indicate an error.

Each process has its own file descriptors table starting from 0. The table initiates with the three standard file descriptors, corresponding to the three standard streams:

| Value | Name              | <unistd.h> symbolic constant | <stdio.h> file stream |
|-------|-------------------|------------------------------|-----------------------|
| 0     | Standard input    | STDIN_FILENO                 | stdin                 |
| 1     | Standard output   | STDOUT_FILENO                | stdout                |
| 2     | Standard error    | STDERR_FILENO                | stderr                |

You might have noticed that the LIBC file descriptor `FILE*` you get when you open a file is not an unsigned int, but rather a struct poniter. This is because LIBC keeps some extra metadata about the opened file in the heap, but the kernel only really cares about the file descriptor number.

The number of file descriptors available to each process is limited by the `OPEN_MAX` constant defined in `<sys/limits.h>`.

In the kernel, file descriptors are saved in their corresponding process structure as a linked list. The file descriptor structure is defined as follows:

```c
typedef struct {
    int fileno;             // file descriptor number
    uint32_t offset         // the current position within the file
    uint32_t flags;         // flags which have been used to open the file
    inode_t inode;          // the inode of the file

    struct fd_t* next;
    struct fd_t* prev;
} fd_t;
```