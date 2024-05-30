# File System Interface

from fs_structs import *
from disk import *
from bitfield import *
import os

class FileSystem:

    def __init__(self):
        self._super    = SuperBlock()
        self._disk     = Disk(Consts.IMAGE_PATH, self.fs_size)
        self._root     = Inode("/", InodeType.DIR)
        self._inodemap = BitField(self._super.inode_count)
        self._blockmap = BitField(self._super.block_count)

        # Mark used inodes and blocks
        self._inodemap.set(0, 1)
        for i in range(1 + self._super.blockmap_count + self._super.inodemap_count
                       + self._super.inode_blocks_count):
            self._blockmap.set(i, 1)

    @property
    def fs_size(self) -> int:
        ''' Get the size of the file system. '''
        return self._super.block_count * self._super.block_size
    

    def _get_inode_offset(self, index: int) -> int:
        ''' Get the disk offset of an inode by its index. '''
        return int(self._super.inode_start + index * (self._super.block_size/self._super.inodes_per_block))


    def _write_inode(self, inode: Inode) -> int:
        ''' Write an inode at an available location in the disk and return its index. '''
        index  = self._inodemap.get()
        offset = self._get_inode_offset(index)
        self._disk.write(inode.binary, offset) 

        return index
        

    def _get_block_offset(self, index: int) -> int:
        ''' Get the disk offset of a block by its index. '''
        return self._super.block_size * index


    def _write_file(self, path: str) -> tuple[list[int], int]:
        ''' Write a given file into the disk and return a tuple containing
            a list of blocks to which it was written to, and the file's size. '''
        if os.path.isdir(path):
            raise ValueError("Can't write a directory into the disk.")
        
        blocks = []
        fsize   = 0
        with open(path, "rb") as f:
            data = f.read(self._super.block_size)
            while data != b'':
                index  = self._blockmap.get()
                offset = self._get_block_offset(index)
                self._disk.write(data, offset)
                blocks.append(index)
                data = f.read(self._super.block_size)

        return (blocks, fsize)


    def _flush(self) -> None:
        ''' Flush the file system into the disk. '''
        print("Flushing meta.")
        self._disk.write(self._super.binary, 0)
        self._disk.write(self._blockmap.binary, self._super.blockmap_start)
        self._disk.write(self._inodemap.binary, self._super.inodemap_start)


    def foo(self, parent: Inode, path: str):

        # Iterate over all the files in the directory
        for f in [os.path.join(path, f) for f in os.listdir(path)]:
            print(f"Reading {f}.")
            # If file is a directory, scan it recursively and link it to parent
            if os.path.isdir(f):
                dir = Inode(os.path.basename(f), InodeType.DIR)
                parent.link(self._write_inode(self.foo(dir, f)))
            # If its a normal file, write its contents to the disk and link it to parent
            else:
                file = Inode(os.path.basename(f), InodeType.FILE)
                blocks, fsize = self._write_file(f)
                file.link(blocks, fsize)
                parent.link(self._write_inode(file))

        return parent


    def mount(self, path: str) -> None:
        if not os.path.isdir(path):
            raise ValueError("File system can only mount directories, not files.")
        
        self._root = self.foo(self._root, path)
        self._disk.write(self._root.binary, self._get_inode_offset(0))
        self._flush()