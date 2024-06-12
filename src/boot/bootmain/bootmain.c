// Bootmain // ~ eylon

#include <kernel/process/elf.h>
#include <libc/stddef.h>
#include "baby_fs.h"
#include <drivers/pata.h>
#include <libc/string.h>

#define KERNEL_PATH "sys/kernel.elf"

// Bootmain entry; load the kernel
void bootmain()
{
    elfheader_t elfhdr;
    int kinode;

    // Initiate the baby file system and get the kernel's inode index
    init_baby_fs();
    kinode = baby_fs_seek(KERNEL_PATH);

    if (kinode < 0) {
        for (;;) __asm__ volatile("hlt");
    }
    
    // Read the kernel's ELF header and check its validity
    baby_fs_read(kinode, &elfhdr, sizeof(elfheader_t), 0);
    if (elfhdr.identify.magic != ELF_MAGIC      // ELF magic
        || elfhdr.identify.bitness != 1         // 32 bit executable
        || elfhdr.identify.abi != 0             // System-V
        || elfhdr.machine != 3)                 // x86
    {
        for (;;) __asm__ volatile("hlt");
    }

    /* Load the ELF's segments.
        Read all program headers and load their segments. */
    for (size_t i = 0; i < elfhdr.phnum;i++)
    {
        prgheader_t prghdr;

        // Read the program header
        baby_fs_read(kinode, &prghdr, sizeof(prgheader_t), elfhdr.phoff + elfhdr.phsize*i);

        // read segment from disk to memory
        baby_fs_read(kinode, (void*) prghdr.paddr, prghdr.filesz, prghdr.offset);

        // if the segment file size is less than the segment memory size fill the undefined area with zeros
        if (prghdr.filesz < prghdr.memsz) {
            // zero out undefined area
            memset((void*) (prghdr.paddr + prghdr.filesz), 0, prghdr.memsz - prghdr.filesz);
        }
    }

    // Jump to the kernel's entry
    void (*entry)() = (void*) elfhdr.entry;
    entry();
    
    // This code should never run
    for (;;) __asm__ volatile("hlt");
}