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
#include <libc/dirent.h>

static superblock_t super;
static paddr_t fs_phys_scratch;

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
    Get file's inode index by its path. Returns -1 if the file could not be found. */
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
    On success, returns the new file's inode index; on failure, returns -1. */
int fs_create(const char* path, inode_type_t type)
{
    int indx_p, indx_c;
    inode_t parent, child;

    // If the file's name is too long, return -1
    if (strlen(basename(path)) >= FNAME_LEN_MAX) {
        return -1;
    }

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
/* Remove a file and its contents from the file system. If the file is a regular file, the function
    frees all of its data blocks and unlinks it from the parent. If the file is a directory,
    the function recursively delete every directory and file in it. On success, 0 is returned, on failure, -1. */
static int iremove(int cindx)
{
    int pindx;
    inode_t inode = inode_read(cindx);

    // If the file is a regular file, free its data blocks
    if (inode.type == FS_NT_FILE)
    {
        for (size_t i = 0; i <= inode.count / super.block_size; i++) {
            blockmap_set(inode.direct[i], 0);
        }
    }
    // If the file is a directory, remove all of its entries
    else if (inode.type == FS_NT_DIR)
    {
        for (size_t i = 0; i < inode.count; i++) {
            iremove(inode.direct[i]);
        }
    }

    // Get the parent directory of the file
    pindx = inode.pindx;
    inode = inode_read(pindx);

    // Unlink the child from its parent
    inode_unlink(&inode, cindx);

    // Decrease the parent's count and update its inode in the disk
    inode.count--;
    inode_write(inode, pindx);

    // Free the child's inode
    inodemap_set(cindx, 0);

    return 0;
}
int fs_remove(const char* path)
{
    int indx = fs_seek(path);
    if (indx < 0) {
        return -1;
    }
    iremove(indx);
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

    // [TODO] create file if not exists and O_CREAT in flags
    // Seek the file's inode; if the file could not be found, return -1
    index = fs_seek(path);
    if (index < 0) {
        return -1;
    }

    // Allocate and set the file descriptor
    fd = (fd_t*) kmalloc(sizeof(fd_t));
    fd->fileno = fileno;
    fd->flags  = flags;
    fd->inode_indx  = index;
    fd->offset = 0;

    // Get the active process and add the new file descriptors to its list
    p = pm_get_active();
    LIST_ADD_END(p->fds, fd);

    fileno++;
    return fileno-1;
}

/* Close an open file descriptor of the currently running process.
    On success returns 0, on failure returns -1.
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

/* If the file is a regular file, the function attempts to read [count]
    bytes from file descriptor [fd] into [buff]. The reading starts from
    seek-offset bytes into the file and stops after reading [count] bytes, or 
    at the end of the file. [buff] should be large enough to contain atleast [count] bytes.
    On success, the number of bytes read is returned (zero indicates end of
    file), and the file position is advanced by this number. On failure, -1 is returned.

    If the file is a directory, the function attempts to read [count] file entries from
    file descriptor [fd] into [buff]. The reading starts at the entry with index seek-offset
    and stops after reading [count] entries, or until there are no more entries to read.
    [buff] should be large enough to contain atleast [count]*sizeof(struct dirent) bytes.
    On success, the number of entries read is returned (zero indicates end of directory),
    and the file position is advanced by this number. On failure, -1 is returned. */
ssize_t fs_read(int fd, void* buff, size_t count)
{
    /* Handle standard streams */
    switch (fd)
    {
    case 0:     // STDIN
        return ui_stdin_read(buff, count);
    case 1:     // STDOUT
        return -1;
    case 2:     // STDERR
        return -1;
    }

    /* Handle file streams */
    fd_t* fd_p;
    inode_t inode;

    // Check if the file is open
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }

    // Get the file's inode
    inode = inode_read(fd_p->inode_indx);

    // If inode is a regular file
    if (inode.type == FS_NT_FILE)
    {
        vaddr_t scratch;
        size_t bytes_read = 0;

        scratch = vmm_attach_page(fs_phys_scratch); // attach scratch space

        // Go over the inode's direct list starting from the block at offset [offset]
        for (size_t i = fd_p->offset / super.block_size; i <= inode.count / super.block_size; i++)
        {
            size_t offset, size;

            // Calculate the offset within the block and the size to copy
            offset = scratch + (fd_p->offset % super.block_size);
            size   = count - bytes_read;
            if (offset + size > scratch + super.block_size) {
                size = (scratch+super.block_size) - offset;
            }

            // Read the block and copy it into the buffer
            block_read((void*) scratch, inode.direct[i]);
            memcpy((void*) ((size_t) buff + bytes_read), (void*) offset, size);

            fd_p->offset += size;
            bytes_read   += size;

            // If read [count] bytes, exit
            if (bytes_read >= count) {
                break;
            }
        }

        vmm_detach_page(scratch);
        return bytes_read;
    }
    // If Inode is a directory
    else if (inode.type == FS_NT_DIR)
    {
        size_t files_read = 0;
        struct dirent* entbuff = (struct dirent*) buff;

        // Go over the inode's direct list starting from the inode at offset [offset]
        for (size_t i = fd_p->offset; i < inode.count; i++)
        {
            inode_t entinode;
            
            // Read inode of the directory entry
            entinode = inode_read(inode.direct[i]);

            // Define the dirnet structure
            strcpy(entbuff[i].d_name, entinode.name);
            entbuff[i].d_type = (entinode.type == FS_NT_FILE ? DT_REG : DT_DIR);

            files_read++;

            if (files_read >= count) {
                break;
            }
        }
        

        return files_read;
    }

    return -1;
}


// int fs_rename(const char *oldpath, const char *newpath)
// {
//     char dirname_old[PATH_MAX];
//     char* dirname_new;
//     int indx;
//     inode_t inode;
    
//     // Try to get the file's inode; if file does not exist, return -1
//     indx = fs_seek(oldpath);;
//     if (indx < 0) {
//         return -1;
//     }
//     inode = inode_read(indx);

//     // Get the direcotry name of the old and new paths
//     strcpy(dirname_old, dirname(oldpath));
//     dirname_new = dirname(newpath);

//     // Compare the old directory with the new one; if they are not the same, move the inode into the new directory
//     if (strcmp(dirname_old, dirname_new) != 0)
//     {
        
//     }

//     // Change the inode's name
    
// }

/* Attempt to write [count] bytes to file descriptor [fd] from [buff].
    The writing starts from the seek offset in the file and stops after
        writing [count] bytes. The function my truncate the file to fit
        the data.
    On success, the number of bytes written is returned, and the file position
    is advanced by this number. On failure, -1 is returned.*/
ssize_t fs_write(int fd, const void* buff, size_t count)
{
    /* Handle standard streams */
    switch (fd)
    {
    case 0:     // STDIN
        return -1;
    case 1:     // STDOUT
        return ui_stdout_write((const char*) buff, count);
    case 2:     // STDERR
        return 0;
    }

    /* Handle file streams */
    fd_t* fd_p;
    vaddr_t scratch;
    inode_t inode;
    size_t bytes_written = 0;

    // Check if the file is open
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }

    inode = inode_read(fd_p->inode_indx);           // get the file's inode
    scratch = vmm_attach_page(fs_phys_scratch);     // attach scratch space

    // Trunacte the file to fit the new data if needed
    if (fd_p->offset + count > inode.count) {
        fs_ftruncate(fd, fd_p->offset+count);
        inode = inode_read(fd_p->inode_indx);

    }

    // Go over the inode's direct list starting from the block at offset [offset]
    for (size_t i = fd_p->offset / super.block_size; i < FS_DIRECT_NUM; i++)
    {
        size_t offset, size;

        // Calculate the offset within the block and the size to write to it
        offset = scratch + (fd_p->offset % super.block_size);
        size   = count - bytes_written;
        if (offset + size > scratch + super.block_size) {
            size = (scratch+super.block_size) - offset;
        }

        // Read the block, copy the buffer into it and write the block back into the disk
        block_read((void*) scratch, inode.direct[i]);
        memcpy((void*) offset, (void*) ((size_t) buff + bytes_written), size);
        block_write((void*) scratch, inode.direct[i]);

        fd_p->offset  += size;
        bytes_written += size;

        // If read [count] bytes exit
        if (bytes_written >= count) {
            break;
        }
    }

    vmm_detach_page(scratch);
    return bytes_written;
}


/* The fs_truncate() and fs_ftruncate() functions cause the regular file
    named by [path] or referenced by [fd] to be truncated to a size of
    precisely [length] bytes.
    
    If the file previously was larger than this size, the extra data
    is lost. If the file previously was shorter, it is extended, and
    the extended part reads as null bytes ('\0').

    With ftruncate(), the file must be open for writing, and the file offset is not changed.

    On success, zero is returned.  On error, -1 is returned, and
    errno is set to indicate the error.*/
static int itruncate(inode_t inode, int index, size_t length)
{
    // Deallocate blocks if decreasing the file's size
    for (size_t i = inode.count / super.block_size; i > length / super.block_size; i--)
    {
        blockmap_set(inode.direct[i], 0);
    }
    // Allocate blocks if increasing the file's size; zero out the blocks.
    if ((inode.count / super.block_size < length / super.block_size) || (inode.count / super.block_size == 0))
    {
        // Create a null buffer
        vaddr_t scratch = vmm_attach_page(fs_phys_scratch);
        memset((void*) scratch, 0, super.block_size);

        // Allocate blocks while copying the null buffer into them
        for (size_t i = inode.count / super.block_size; i < length / super.block_size || i == 0; i++)
        {
            inode.direct[i] = blockmap_get();
            block_write((void*) scratch, inode.direct[i]);
        }
        vmm_detach_page(scratch);
    }

    inode.count = length;
    inode_write(inode, index);

    return 0;
}
int fs_truncate(const char* path, size_t length)
{
    inode_t inode;
    int index;

    index = fs_seek(path);
    if (index < 0) {
        return -1;
    }
    inode = inode_read(index);

    return itruncate(inode, index, length);

}
int fs_ftruncate(int fd, size_t length)
{
    fd_t* fd_p;
    inode_t inode;
    
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }
    inode = inode_read(fd_p->inode_indx);

    return itruncate(inode, fd_p->inode_indx, length);
}


/* The fs_stat() and fs_fstat() functions retrieve information about the regular file
    named by [path] or referenced by [fd].
        Inormation is stored in the stat struct buffer [buf].
    On success, zero is returned, on failure, -1 is returned. */
static int fs_istat(int inode_indx, struct stat* buf)
{
    inode_t inode = inode_read(inode_indx);

    buf->st_size = inode.count;
    buf->type    = inode.type == FS_NT_FILE ? DT_REG : DT_DIR;
    buf->indx    = inode_indx;

    return 0;
}
int fs_stat(const char *path, struct stat* buf)
{
    int index;

    index = fs_seek(path);
    if (index < 0) {
        return -1;
    }
    
    return fs_istat(index, buf);
}
int fs_fstat(int fd, struct stat* buf)
{
    fd_t* fd_p;
    
    fd_p = fd_seek(fd);
    if (fd_p == NULL) {
        return -1;
    }

    return fs_istat(fd_p->inode_indx, buf);
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
            size_t fsize = inode_read(fd_p->inode_indx).count;
            if ((int) fsize + offset < 0) { break; }
            fd_p->offset = fsize + offset;
            return fd_p->offset;
    }

    return 1;
}

/* Construct a path null terminated string from an inode index. The function takes
    [inode_indx], which is the index of the inode to which we want to find the path,
    and [buff], which is a [size] bytes long buffer that will hold the path string.

    If length of the absolute path of the inode exceeds [size], NULL is returned and
    the string in [buff] is undefined.

    On success, [buff] is returned. On error NULL is returned. */
char* fs_build_path(int indx, char* buff, size_t size)
{
    size_t pathlen = 0;
    buff[--size] = '\0';
    
    // Step until reaching the root directory
    while (indx != FS_ROOT_INDEX)
    {
        inode_t inode  = inode_read(indx);          // read inode
        size_t namelen = strlen(inode.name);        // get string length
        pathlen += namelen+1;                       // add string length plus '/' separator to the path length

        // If path string is too long to fit in [buff], return NULL
        if (pathlen > size) {
            return NULL;
        }

        // Copy the inode name into the buffer
        buff[size-pathlen] = FS_SPLIT_CHAR[0];
        memcpy(buff + (size-pathlen+1), inode.name, namelen);

        indx = inode.pindx;                   // get the inode index of the current inode's parnet
    }
    memcpy(buff, buff + size-pathlen, pathlen+1);

    return buff;
}

// Initiate the file system
void init_fs()
{
    fs_phys_scratch = pmm_get_page();   // allocate a physical scratch space for the file system
    super_read();                       // read the superblock
}