// File System // ~ eylon

#include <kernel/fs.h>
#include <drivers/pata.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/heap.h>
#include <kernel/panic.h>
#include <kernel/process/pm.h>
#include <libc/stddef.h>
#include <libc/stdint.h>
#include <libc/list.h>
#include <libc/fcntl.h>
#include <libc/stdbool.h>
#include <libc/bitfield.h>
#include <libc/string.h>
#include <libc/sys/stat.h>
#include <libc/unistd.h>

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
static void super_read()
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
static inline void block_read(void* dest, size_t index)
{
    pata_read_disk(dest,
        super.block_size/PATA_SECTOR_SIZE,
        FS_START_SECTOR + index*(super.block_size/PATA_SECTOR_SIZE));
}

// Write to the block at index [index] from [src]
static inline void block_write(void* src, size_t index)
{
    pata_write_disk(src,
        super.block_size/PATA_SECTOR_SIZE,
        FS_START_SECTOR + index*(super.block_size/PATA_SECTOR_SIZE));
}


// Read an inode by its index
static inode_t inode_read(size_t index)
{
    inode_t inode;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    block_read((void*) scratch, INODE_BLOCK(index));
    inode = *(inode_t*) (scratch + INODE_OFFSET(index));

    vmm_detach_page(scratch);
    return inode;
}

// Write an inode into an inode index
static void inode_write(inode_t inode, size_t index)
{
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    block_read((void*) scratch, INODE_BLOCK(index));
    *(inode_t*) (scratch + INODE_OFFSET(index)) = inode;
    block_write((void*) scratch, INODE_BLOCK(index));

    vmm_detach_page(scratch);
}

/* Link an index into an available location in an inode's direct list.
    On success returns 0, on failure returns non zero.
    IMPORTANT: function does not affect the disk directly; caller is
        responsible for updating the inode in the disk. */
static int inode_link(inode_t* inode, size_t link)
{
    for (size_t i = 0; i < sizeof(inode->direct) * sizeof(inode->direct[0]); i++)
    {
        if (inode->direct[i] == 0) {
            inode->direct[i] = link;
            return 0;
        }
    }
    return -1;
}

/* Unlink an index from an inode's direct list.
    On success returns 0, on failure returns non zero.
    WARNING: function does not affect the disk directly; caller is
        responsible for updating the inode in the disk. */
static int inode_unlink(inode_t* inode, size_t link)
{
    for (size_t i = 0; i < sizeof(inode->direct) * sizeof(inode->direct[0]); i++)
    {
        if (inode->direct[i] == link) {
            inode->direct[i] = 0;
            return 0;
        }
    }
    return -1;
}

// Set the block at index [index] to [value] in the blockmap
static void blockmap_set(size_t index, bool value)
{
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    block_read((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));
    BITFIELD_SET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8, value);
    block_write((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
}

// Get the status of the block at index [index] from the blockmap
static bool blockmap_check(size_t index)
{
    bool value;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    block_read((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));
    value = BITFIELD_GET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8);
    block_write((void*) scratch, BLOCKMAP_START + (index/(super.block_size*8)));

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

    block_read((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));
    BITFIELD_SET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8, value);
    block_write((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));

    vmm_detach_page(scratch);
}

// Get the status of the inode at index [index] from the inodemap
static bool inodemap_check(size_t index)
{
    bool value;
    vaddr_t scratch = vmm_attach_page(fs_phys_scratch);

    block_read((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));
    value = BITFIELD_GET_BIT(*(uint8_t*) (scratch + ((index/8) % super.block_size)), index%8);
    block_write((void*) scratch, INODEMAP_START + (index/(super.block_size*8)));

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

/* Get a file descriptor pointer of the currently running process by its number.
    Returns a pointer to the file descriptor structure or NULL if not found. */
static fd_t* fd_seek(int fd)
{
    fd_t* fd_p = pm_get_active()->fds;

    while (fd_p != NULL)
    {
        if (fd_p->fileno == fd) {
            break;
        }
        fd_p = fd_p->next;
    }

    return fd_p;
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

    parent = inode_read(FS_ROOT_INDEX);         // get the root directory's inode
    token = strtok(path_cp, FS_SPLIT_CHAR);     // get the first token

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

    kfree(path_cp);
    return index;
}

/* Create a new file.
    On success, returns the new file's inode index; on failure, returns a negative number. */
int fs_create(const char* path, inode_type_t type)
{
    int indx_p, indx_c;
    inode_t parent, child;

    // Get the parent directory of the new file; if it does not exist return -1
    indx_p = fs_seek(dirname(path));
    if (indx_p < 0) {
        return -1;
    }
    parent = inode_read(indx_p);
    if (parent.type != FS_NT_DIR) {
        return -1;
    }

    // Set the child's inode
    strcpy(child.name, basename(path));
    child.type  = type;
    child.count = 0;
    memset(child.direct, 0, sizeof(child.direct) * sizeof(child.direct[0]));
    
    // Get a free inode index and load the child's inode into it
    indx_c = inodemap_get();
    inode_write(child, indx_c);

    // Link the child to its parent; if linkage fails return -1
    if (inode_link(&parent, indx_c) != 0) {
        return -1;
    }
    // Increase the parent's count and update its inode in the disk
    parent.count++;
    inode_write(parent, indx_p);

    return indx_c;
}

/* Delete a file.
    On success, returns 0; on failure, returns non-zero.
    WARNING: be careful about deleting directories unrecursively or deleting files
        without freeing their data blocks first. */
int fs_remove(const char* path)
{
    int indx_p, indx_c;
    inode_t parent;

    // Get the child's inode index; if it does not exist return -1
    indx_c = fs_seek(path);
    if (indx_c < 0) {
        return -1;
    }

    // Get the parent directory of the file
    indx_p = fs_seek(dirname(path));
    parent = inode_read(indx_p);

    // Unlink the child from its parent; if unlink succeed,
    inode_unlink(&parent, indx_c);

    // Decrease the parent's count and update its inode in the disk
    parent.count--;
    inode_write(parent, indx_p);

    // Free the child's inode
    inodemap_set(indx_c, 0);

    return 0;
}

/* Create a file descriptor for the given path and associate it with the
    currently running process.
    On success returns the new file descriptor; on failure returns a negative value. */
int fs_open(const char* path, int flags)
{
    static size_t fileno = 3;   // file descriptor number counter

    fd_t* fd;
    process_t* p;
    int index;

    // Seek the file's inode; if the file could not be found, return -1
    index = fs_seek(path);
    if (index < 0) {
        return -1;
    }

    // Allocate and set the file descriptor
    fd = (fd_t*) kmalloc(sizeof(fd_t));
    fd->fileno = fileno;
    fd->flags  = flags;
    fd->inode  = index;
    fd->offset = 0;

    // Get the active process and add the new file descriptors to its list
    p = pm_get_active();
    LIST_ADD_END(p->fds, fd);

    fileno++;
    return fileno-1;
}

/* Close an open file descriptor of the currently running process.
    On success returns 0, on failure returns a negative number.
    WARNING: the three standard file descriptors (stdin=0, stdout=1,
        and stderr=2) cannot be closed. */
int fs_close(int fd)
{
    process_t* p;
    fd_t* fd_p;

    // If the file descriptor is a standard stream descriptor abort
    if (fd < 3) {
        return -1;
    }

    // Get the active process and seek its file descriptors list
    p = pm_get_active();
    fd_p = p->fds;
    while (fd_p != NULL) {
        // If found the file descriptor, remove it from the list and return
        if (fd_p->fileno == fd) {
            LIST_REMOVE(p->fds, fd_p);
            return 0;
        }
        fd_p = fd_p->next;  // step the list
    }

    // No such file descriptor
    return -1;
}

/* Attempt to read [count] bytes from file descriptor [fd] into [buff].
    The reading starts from the seek offset in the file and stops after
        reading [count] bytes, or at the end of the file.
    On success, the number of bytes read is returned (zero indicates
        end of file), and the file position is advanced by this number,
        on failure, -1 is returned.
    NOTE: read function only works on files, not directories. */
ssize_t fs_read(int fd, void* buff, size_t count)
{
    /* Handle standard streams */
    switch (fd)
    {
    case 0:     // STDIN
        return 0;
    case 1:     // STDOUT
        return 0;
    case 2:     // STDERR
        return 0;
    }

    /* Handle file streams */
    fd_t* fd_p;
    vaddr_t scratch;
    inode_t inode;
    size_t bytes_read = 0;

    // Check if the file is open
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }

    inode = inode_read(fd_p->inode);            // get the file's inode
    scratch = vmm_attach_page(fs_phys_scratch); // attach temporary scratch space

    // Go over the inode's direct list
    for (size_t i = fd_p->offset / super.block_size; i <= inode.count / super.block_size; i++)
    {
        size_t offset, size;

        // Calculate the offset within the block and the size to copy
        offset = scratch + (fd_p->offset % super.block_size);
        size   = offset - scratch;
        if (size == 0) {
            size = (count - bytes_read);
            if (size > super.block_size) {
                size = super.block_size;
            }
        }

        // Read the block and copy it into the buffer
        block_read((void*) scratch, inode.direct[i]);
        memcpy(buff, (void*) offset, size);

        fd_p->offset += size;
        bytes_read   += size;

        // If read [count] bytes exit
        if (bytes_read >= count) {
            break;
        }
    }

    vmm_detach_page(scratch);                   // detach temporary scratch space
    return bytes_read;
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

/* Retrieve information about the file pointed to by [path].
    Inormation is stored in the stat struct buffer [buf].
    On success, zero is returned, on failure, a non-zero number is returned. */
int fs_stat(const char *path, struct stat *buf)
{
    inode_t inode;
    int index;

    index = fs_seek(path);
    if (index < 0) {
        return -1;
    }
    inode = inode_read(index);

    buf->st_size = inode.count;

    return 0;
}

/* Retrieve information about the the open file descriptor [fd].
    Inormation is stored in the stat struct buffer [buf].
    On success, zero is returned, on failure, a non-zero number is returned. */
int fs_fstat(int fd, struct stat *buf)
{
    fd_t* fd_p;
    inode_t inode;
    
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }
    inode = inode_read(fd_p->inode);

    buf->st_size = inode.count;

    return 0;
}

/* Reposition the file offset of an open file by [offset] and according
    to the value of [whence] as follows:
    SEEK_SET: The file offset is set to offset bytes.
    SEEK_CUR: The file offset is set to its current location plus offset bytes.
    SEEK_END: The file offset is set to the size of the file plus offset bytes.
    On success, returns the resulting offset location as measured in bytes from
    the beginning of the file, on failure, -1 is returned. */
int fs_lseek(int fd, int offset, int whence)
{
    // Get file descriptor and check if it is valid
    fd_t* fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }

    // Set offset according to [whence]
    switch (whence)
    {
        case SEEK_SET:
            if (offset < 0) { break; }
            fd_p->offset = offset;
            return fd_p->offset;

        case SEEK_CUR:
            if ((int) fd_p->offset + offset < 0) { break; }
            fd_p->offset = fd_p->offset + offset;
            return fd_p->offset;

        case SEEK_END:
            size_t fsize = inode_read(fd_p->inode).count;
            if ((int) fsize + offset < 0) { break; }
            fd_p->offset = fsize + offset;
            return fd_p->offset;
    }

    return 1;
}

// Initiate the file system
void init_fs()
{
    fs_phys_scratch = pmm_get_page();   // allocate a physical scratch space for the file system
    super_read();                       // read the superblock

    blockmap_get();     // DEBUG
}