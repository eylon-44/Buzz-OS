// File System // ~ eylon

#include <kernel/fs.h>
#include <drivers/pata.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/heap.h>
#include <kernel/panic.h>
#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/stdbool.h>
#include <libc/bitfield.h>
#include <libc/string.h>

static superblock_t super;
static paddr_t fs_phys_scratch;

// Divide an inteager while rounding it up
#define DIV_INT_UP(dividend, divisor) ((dividend + (divisor-1))/divisor)
// Number of inodes in the file system
#define INODE_COUNT     (super.inode_blocks_count * super.inodes_per_block)
// Number of blockmap blocks in the file system
#define BLOCKMAP_COUNT  (DIV_INT_UP(DIV_INT_UP(super.block_count, super.block_size), 8))
// Block index at which the blockmap starts
#define BLOCKMAP_START  (1)
// Number of inodemap blocks in the file system
#define INODEMAP_COUNT  (DIV_INT_UP(DIV_INT_UP(INODE_COUNT, super.block_size), 8))
// Block index at which the inodemap starts
#define INODEMAP_START  (1 + BLOCKMAP_COUNT)
// Block index at which the inode list starts
#define INODE_START     (1 + BLOCKMAP_COUNT + INODEMAP_COUNT)

// Get the block index at which a certain inode is in by its index
#define INODE_BLOCK(index)  (INODE_START + (index/super.inodes_per_block))
// Get the inode offset inside the block by its index
#define INODE_OFFSET(index) ((index%super.inodes_per_block) * (super.block_size/super.inodes_per_block))


// Read the superblock
static void read_super()
{
    // Allocate a temporary scratch space to load the superblock into
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    // Read the superblock
    pata_read_disk((void*) scratch, 1, FS_START_SECTOR);
    super = *(superblock_t*) scratch;

    // Make sure we have found the superblock
    if (super.fs_magic != FS_MAGIC) {
        KPANIC("File system: superblock not found.");
    }

    // Free temporary scratch space
    vmm_detach_page(scratch);
}


// Read the block at index [index] into [dest]
static inline void read_block(void* dest, size_t index)
{
    pata_read_disk(dest,
        super.block_size/PATA_SECTOR_SIZE,
        FS_START_SECTOR + index*(super.block_size/PATA_SECTOR_SIZE));
}

// Write to the block at index [index] from [src]
static inline void write_block(void* src, size_t index)
{
    pata_write_disk(src,
        super.block_size/PATA_SECTOR_SIZE,
        FS_START_SECTOR + index*(super.block_size/PATA_SECTOR_SIZE));
}


// Read an inode by its index
static inode_t read_inode(size_t index)
{
    inode_t inode;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, INODE_BLOCK(index));
    inode = *(inode_t*) (scratch + INODE_OFFSET(index));

    vmm_detach_page(scratch);
    return inode;
}

// Write an inode into an inode index
static void write_inode(inode_t inode, size_t index)
{
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, INODE_BLOCK(index));
    *(inode_t*) (scratch + INODE_OFFSET(index)) = inode;
    write_block((void*) scratch, INODE_BLOCK(index));

    vmm_detach_page(scratch);
}


// Set the block at index [index] to [value] in the blockmap
static void blockmap_set(size_t index, bool value)
{
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));
    BITFIELD_SET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8, value);
    write_block((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
}

// Get the status of the block at index [index] from the blockmap
static bool blockmap_check(size_t index)
{
    bool value;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));
    value = BITFIELD_GET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8);
    write_block((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
    return value;
}

// Get the index of a free block in the blockmap while marking it as used
static size_t blockmap_get()
{
    static size_t seek = 0;

    while (blockmap_check(seek)) {
        seek = (seek+1) % super.block_count;
    }
    blockmap_set(seek, 1);

    return seek;
}


// Set the inode at index [index] to [value] in the inodemap
static void inodemap_set(size_t index, bool value)
{
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));
    BITFIELD_SET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8, value);
    write_block((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
}

// Get the status of the inode at index [index] from the inodemap
static bool inodemap_check(size_t index)
{
    bool value;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    read_block((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));
    value = BITFIELD_GET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8);
    write_block((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
    return value;
}

// Get the index of a free inode in the inodemap while marking it as used
static size_t inodemap_get()
{
    static size_t seek = 0;

    while (inodemap_check(seek)) {
        seek = (seek+1) % INODE_COUNT;
    }
    inodemap_set(seek, 1);

    return seek;
}


/* Seek a file.
    Get file's inode index by its path. Returns a negative value if
    the file could not be found. */
int fs_seek(const char* path)
{
    char *path_cp, *token;
    inode_t parent;
    int index = -1;

    // Preserve the [path] string as strtok() corrupts it
    path_cp = (char*) kmalloc(strlen(path)+1);
    strcpy(path_cp, path);

    parent = read_inode(FS_ROOT_INDEX);         // get the root directory's inode
    token = strtok(path_cp, FS_SPLIT_CHAR);     // get the first token

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
            child = read_inode(index);

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

    kfree(path_cp);
    return index;
}

/* Create a new file.
    On success, return 0; any other value indicates an error. */
int fs_create(const char* path)
{
    return 0;
}

/* Create a new firectory.
    On success, return 0; any other value indicates an error. */
int fs_mkdir(const char* path)
{
    return 0;
}

void fs_write(int fd)
{
    /* Handle standard streams */
    switch (fd)
    {
    case 0:     // STDIN
        return;
    case 1:     // STDOUT
        return;
    case 2:     // STDERR
        return;
    }

    /* Handle file streams */
    // get fd linked list from the active process
}


// Initiate the file system
void init_fs()
{
    fs_phys_scratch = pmm_get_page();   // allocate a physical scratch space for the file system
    read_super();                       // read the superblock
}