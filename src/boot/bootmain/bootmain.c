// Bootmain // ~ eylon

#include <kernel/process/elf.h>
#include <drivers/pata.h>
#include <libc/stddef.h>

// Kernel start address in disk
#define DISK_KERNEL_START (21 * PATA_SECTOR_SIZE)
// Scratch space to place the kernel ELF data while loading it
#define SCRATCH_SPACE 0x10000
// Page size
#define PAGE_SIZE 0x1000

// Bootmain entry
void bootmain()
{
    uint32_t filesz UNUSED;
    prgheader_t* prgheader = NULL;
    // Scratch space pointer to load the ELF data into
    elfheader_t* elfheader = (elfheader_t*) SCRATCH_SPACE;

    // Load a page from the kernel's ELF into memory
    pata_read_disk((void*) elfheader, PAGE_SIZE, DISK_KERNEL_START);

    // Check for the ELF magic; return to the bootsector if there is no match
    if (elfheader->identify.magic != ELF_MAGIC)
        return;

    // ELF file size; [start of section headers] + ([section header size] * [number of section headers])
    filesz = elfheader->shoff + (elfheader->shsize * elfheader->shnum);

    // Go over all the program headers and load their segments
    prgheader = (prgheader_t*) ((uint8_t*) elfheader + elfheader->phoff);
    for (size_t i = 0; i < elfheader->phnum; prgheader++, i++)
    {
        // read segment from disk to memory
        pata_read_disk((void*) prgheader->paddr, prgheader->filesz, DISK_KERNEL_START + prgheader->offset);

        // if the segment file size is less than the segment memory size fill the undefined area with zeros
        if (prgheader->filesz < prgheader->memsz) {
            // set undefined area with zero
            for (size_t k = prgheader->paddr + prgheader->filesz;
                    k < prgheader->paddr + prgheader->memsz;
                    k++)
            {
                *((uint8_t*) k) = 0;
            }
        }
    }

    // Jump to the kernel's entry
    void (*entry)() = (void*) elfheader->entry;
    entry();
    
    // This code should never run
    for (;;)
        __asm__ volatile("hlt");
}