// Bootmain // ~ eylon

#include <kernel/process/elf.h>
#include "pata.h"

// Kernel start address in disk
#define DISK_KERNEL_START + 21 * PATA_SECTOR_SIZE
// Scratch space to place the kernel ELF data while loading it
#define SCRATCH_SPACE 0x10000
// Page size
#define PAGE_SIZE 0x1000

// Bootmain entry
void bootmain()
{
    u32_t filesz;
    prgheader_t*  prgheader;
    // Scratch space pointer to load the ELF data into
    elfheader_t* elfheader = (elfheader_t*) SCRATCH_SPACE;

    // Load a page from the kernel's ELF into memory
    read_disk((void*) elfheader, PAGE_SIZE, DISK_KERNEL_START +);

    // Check for the ELF magic; return to the bootsector if there is no match
    if (elfheader->identify.magic != ELF_MAGIC)
        return;

    // ELF file size; [start of section headers] + ([section header size] * [number of section headers])
    filesz = elfheader->shoff + (elfheader->shentsize * elfheader->shnum);
    UNUSED(filesz);

    // Go over all the program headers and load their segments
    for (int i = 0, prgheader = (prgheader*) (DISK_KERNEL_START + elfheader->phoff);
        i < elfheader->phnum;
        prgheader++)
    {
        read_disk(prgheader->paddr, prgheader->filesz, DISK_KERNEL_START + prgheader->offset)

        // set bss sections with 0!!!!
        // and set the entry to physical as it is currently points to a virtual.
        // update the pata drive to round [size] UP for a sector so we won't lose data!!!
    }


    // Jump to the kernel's entry
    void (*entry)() = (void*) elfheader->entry;
    entry();
    
    // This code should never run
    for (;;)
        __asm__ volatile("hlt");
}