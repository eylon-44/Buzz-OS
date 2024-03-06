// Global Descriptor Table Handler // ~ eylon

#include "gdt.h"
#include <libc/stdint.h>

// GDT data structure
static gdt_entry_t gdt[SEG_COUNT] = {
    // null segment
    GDT_ENTRY(0, 0, 0, 0, 0),

    // kernel code segment
    GDT_ENTRY(0x0, 0xfffff, 0,
        GDT_ACCESS_PRESENT | GDT_ACCESS_NON_SYS_SEG | GDT_ACCESS_CODE_SEG | GDT_ACCESS_READABLE_CODE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY),

    // kernel data segment
    GDT_ENTRY(0x0, 0xfffff, 0, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_NON_SYS_SEG | GDT_ACCESS_DATA_SEG | GDT_ACCESS_WRITEABLE_DATA,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY),


    // user code segment
    GDT_ENTRY(0x0, 0xfffff, 3,
        GDT_ACCESS_PRESENT | GDT_ACCESS_NON_SYS_SEG | GDT_ACCESS_CODE_SEG | GDT_ACCESS_READABLE_CODE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY),

    // user data segment
    GDT_ENTRY(0x0, 0xfffff, 3, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_NON_SYS_SEG | GDT_ACCESS_DATA_SEG | GDT_ACCESS_WRITEABLE_DATA,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY)
};
// [TODO] this is also used by the kernel enrty for some reason, fix this later and make it static
// GDT descriptor :: this will be loaded to memory using the lgdt instruction
gdt_descriptor_t gdt_descriptor __attribute__((unused)) = { .gdt_size = sizeof(gdt)-1,
                                           .gdt_address = (uint32_t) &gdt };

// GDT initiator
void init_gdt()
{
    __asm__ volatile ("lgdt %0" : : "m" (gdt_descriptor));
}