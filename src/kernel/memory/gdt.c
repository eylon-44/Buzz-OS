// Global Descriptor Table Handler // ~ eylon

#include <kernel/memory/gdt.h>
#include <kernel/memory/mmlayout.h>
#include <kernel/process/tss.h>
#include <libc/stdint.h>

// GDT data structure
static gdt_entry_t gdt[SEG_COUNT+1] = {
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
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY),

    // TSS segment
    GDT_ENTRY(MM_TSS_START, sizeof(tss_t)-1, 0, 0x89, 0)
};

// GDT descriptor :: this will be loaded to memory using the lgdt instruction
static gdt_descriptor_t gdt_descriptor = { .gdt_size = sizeof(gdt)-1,
                                           .gdt_address = (uint32_t) &gdt };

// GDT initiator
void init_gdt()
{
    __asm__ volatile ("lgdt %0" : : "m" (gdt_descriptor));
}