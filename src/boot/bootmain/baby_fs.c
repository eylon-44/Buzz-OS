// A Minimized Implementation of the Kernel's File System for the Bootloader // ~ eylon

#include "baby_fs.h"
#include <kernel/fs.h>
#include <drivers/pata.h>
#include <libc/stddef.h>
#include <libc/string.h>
#include <libc/stdbool.h>

static superblock_t super;

// Read the superblock
static void super_read()
{
    // Read the superblock
    pata_read_disk((void*) SCRATCH_SPACE, 1, FS_START_SECTOR);
    super = *(superblock_t*) SCRATCH_SPACE;

    // Make sure we have found the superblock
    if (super.fs_magic != FS_MAGIC) {
        for (;;)
            __asm__ volatile("hlt");
    }
}

// Read the block at index [index] into [dest]
static inline void block_read(void* dest, size_t index)
{
    pata_read_disk(dest,
        super.block_size/PATA_SECTOR_SIZE,
        FS_START_SECTOR + index*(super.block_size/PATA_SECTOR_SIZE));
}

// Read an inode by its index
static inode_t inode_read(size_t index)
{
    inode_t inode;

    block_read((void*) SCRATCH_SPACE, INODE_BLOCK(index));
    inode = *(inode_t*) (SCRATCH_SPACE + INODE_OFFSET(index));

    return inode;
}


/* Seek a file.
    Get file's inode index by its path. Returns a negative value if
    the file could not be found. */
int baby_fs_seek(char* path)
{
    char* token;
    inode_t parent;
    int index = -1;

    parent = inode_read(FS_ROOT_INDEX);         // get the root directory's inode
    token = strtok(path, FS_SPLIT_CHAR);     // get the first token

    // If the path is the root directoy return 0
    if (strcmp(path, parent.name) == 0) {
        return 0;
    }

    // Seek the splitted path
    while (token != NULL)
    {
        bool present = false;   // indicate if the [child] exists in the [parent] directory or not
        
        // If parent is not a directory return a negative number
        if (parent.type != FS_NT_DIR) {
            return -1;
        }

        // Go over all the files in the [parent] directory
        for (size_t i = 0; i < parent.count; i++) {
            inode_t child;

            index = parent.direct[i];
            child = inode_read(index);

            // If the [child] exists in the [parent], set it as the new [parent] and set [present] to true
            if (strcmp(child.name, token) == 0) {
                present = true;
                parent  = child;
                break;
            }
        }
        // If file is not present return a negative number
        if (!present) {
            return -1;
        }
        token = strtok(NULL, FS_SPLIT_CHAR);    // get the next token
    }

    return index;
}

/* Read [count] bytes from the file with the specified inode index into [buff].
    The reading starts from byte [offset] in the file and stops after
        reading [count] bytes, or at the end of the file.
    The function returns the number of bytes read from the file.
    NOTE: read function only works on files, not directories. */
ssize_t baby_fs_read(int inode_index, void* buff, size_t count, size_t offset)
{
    inode_t inode;
    size_t bytes_read = 0;

    inode = inode_read(inode_index);  // get the file's inode

    // Go over the inode's direct list
    for (size_t i = offset / super.block_size; i <= inode.count / super.block_size; i++)
    {
        size_t seek, size;

        // Calculate the offset within the block and the size to copy
        seek = SCRATCH_SPACE + (offset % super.block_size);
        size   = count - bytes_read;
        if (seek + size > SCRATCH_SPACE + super.block_size) {
            size = (SCRATCH_SPACE+super.block_size) - seek;
        }

        // Read the block and copy it into the buffer
        block_read((void*) SCRATCH_SPACE, inode.direct[i]);
        memcpy((void*) ((size_t) buff + bytes_read), (void*) SCRATCH_SPACE + (seek % super.block_size), size);

        offset     += size;
        bytes_read += size;

        // If read [count] bytes exit
        if (bytes_read >= count) {
            break;
        }
    }

    return bytes_read;
}

void init_baby_fs()
{
    super_read();
}