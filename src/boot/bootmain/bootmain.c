// Bootmain // ~ eylon

#include <kernel/process/elf.h>
#include "pata.h"

void bootmain()
{
    for (;;)
        __asm__ volatile("hlt");
}