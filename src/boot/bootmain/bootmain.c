// Bootmain // ~ eylon

#include <kernel/process/elf.h>
#include "pata.h"

// Kernel start address in disk
#define KERNEL_DISK_START 21 * PATA_SECTOR_SIZE

void bootmain()
{
    read_disk((void*) 0x4000, 512, KERNEL_DISK_START);
    __asm__ volatile("jmp 0x4000");

    // scratch space
    // This code should never run
    for (;;)
        __asm__ volatile("hlt");
}