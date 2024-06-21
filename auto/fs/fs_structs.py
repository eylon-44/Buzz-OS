# File System Structures

from __future__ import annotations
import struct
import math

from config import Config

class Consts:
    ''' Consts must match the macro definitions in the kernel/fs.h file. '''
    FS_MAX_NAME_LEN = 64
    FS_DIRECT_NUM   = 32


class InodeType:
    ''' InodeType must match the macro definitions in the kernel/fs.h file. '''
    FILE = 0xF
    DIR  = 0xD
    LINK = 0xE
    

class Struct:

    def __init__(self, *format: int) -> None:
        ''' __init__ takes an unlimted amount of arguments, each describes the size of an attribute
            in the struct, where the first argument describes the size of the first attribute in the
            struct, the second argument describes the size of the second attribute in the struct and so on. '''
        self._format = Config.get()["disk"]["endian"]
        for size in format:
            self._format += f"{size}s"


    def pack(self, *attr: int | str | list[int]) -> bytes:
        ''' Get a packed binary struct. '''
        # Convert data to binary
        bin = []
        for i in attr:
            if isinstance(i, int):
                bin.append(i.to_bytes(Config.get()["disk"]["bits"],
                    byteorder="little" if Config.get()["disk"]["endian"] == "<" else "big"))
            elif isinstance(i, list):
                bin.append(struct.pack(f"{len(i)}I", *i))
            elif isinstance(i, str):
                bin.append(i.encode())
            else:
                raise ValueError("Trying to pack an unsupported type.")

        # Pack and return
        return struct.pack(self._format, *bin)


class Inode(Struct):

    '''
    typedef struct {
        char name[FS_MAX_NAME_LEN];         // file name
        inode_type_t type;                  // file type
        size_t count;                       // count of <NT_FILE> bytes of data/<NT_DIR> number of files in direcotry
        size_t direct[FS_DIRECT_NUM];       // direct refrence to <NT_FILE> blocks of data/<NT_DIR> inodes
        size_t pindx;                       // parent index
    } __attribute__((packed)) inode_t;
    '''

    def __init__(self, name : str, type: InodeType, pindx: int) -> None:
        super().__init__(Consts.FS_MAX_NAME_LEN*1, 4, 4, Consts.FS_DIRECT_NUM*4, 4)
        self.name     = name + '\0' * (Consts.FS_MAX_NAME_LEN - len(name))
        self.type     = type
        self.count    = 0
        self.direct   = [0] * Consts.FS_DIRECT_NUM
        self.pindx    = pindx

    def __str__(self) -> str:
        return f"Inode {self.name}: type <{self.type}> count <{self.count}> direct <{self.direct}"

    @property
    def binary(self) -> bytes:
        ''' Get the binary representation of the inode. '''
        return self.pack(self.name, self.type, self.count, self.direct, self.pindx)


    def link(self, index: int | list[int], size=0):
        ''' Link data blocks or inodes into this inode. '''

        # If paramter is not a list, make it so
        if not isinstance(index, list):
            index = [index]

        # If inodes represents a directory
        if self.type == InodeType.DIR:
            for i in index:
                self.direct[self.count] = i
                self.count += 1

        # If inodes represents a file
        elif self.type == InodeType.FILE:
            self.direct[0:len(index)] = index
            self.count = size


class SuperBlock(Struct):

    '''
    typedef struct
    {
        uint32_t block_count;           // total number of blocks in the file system
        uint32_t block_size;            // block size; must be a power of 2 and greater than 512
        uint32_t inode_blocks_count;    // total number of inode blocks; total_inodes = indoes_per_block * inode_block_count
        uint32_t inodes_per_block;      // number of inodes in a single block; must be a power of 2 and greater than 0
        uint32_t super_index;           // superblock block number - starting block index 
        uint32_t fs_magic;              // file system unique magic number
    } __attribute__((packed)) superblock_t;
    '''

    def __init__(self):
        super().__init__(4, 4, 4, 4, 4, 4)

        # Load configurations
        data = Config.get()["superblock"]
        self.block_count        = data["block_count"]
        self.block_size         = data["block_size"]
        self.inode_blocks_count = data["inode_blocks_count"]
        self.inodes_per_block   = data["inodes_per_block"]
        self.super_index        = data["super_index"]
        self.fs_magic           = data["fs_magic"]

    @property
    def inode_count(self) -> int:
        ''' Get the number of inodes in the file system. '''
        return self.inode_blocks_count * self.inodes_per_block

    @property
    def blockmap_count(self) -> int:
        ''' Get the number of blockmap blocks in the file system. '''
        return math.ceil((self.block_count/self.block_size)/8)
    
    @property
    def blockmap_start(self) -> int:
        ''' Get the disk offset at which the first blockmap resides. '''
        return 1 * self.block_size

    @property
    def inodemap_start(self) -> int:
        ''' Get the disk offset at which the first inodemap resides. '''
        return (1+self.blockmap_count) * self.block_size

    @property
    def inodemap_count(self) -> int:
        ''' Get the number of inodemap blocks in the file system. '''
        return math.ceil((self.inode_count/self.block_size)/8)
    
    @property
    def binary(self) -> bytes:
        ''' Get the binary representation of the superblock. '''
        return self.pack(self.block_count, self.block_size, self.inode_blocks_count,
                    self.inodes_per_block, self.super_index, self.fs_magic)
    
    @property
    def inode_start(self) -> int:
        ''' Get the disk offset at which the first inode resides. '''
        return (1+self.blockmap_count+self.inodemap_count) * self.block_size