// TSS Manager //

#include <kernel/process/tss.h>
#include <kernel/memory/gdt.h>
#include <kernel/memory/mmlayout.h>
#include <libc/stddef.h>
#include <libc/string.h>

// TSS entry
static tss_t* tss_entry = (tss_t*) MM_TSS_START;

// Flush (load) the TSS
static inline void tss_flush()
{
    __asm__ volatile ("movw %0, %%ax\n\t"
    "ltr %%ax"
    :
    : "i"(GDT_TSS_SEG)
    : "%ax");
}

// Set a new ESP0:SS0 value for the TSS
void tss_set_stack(size_t esp0, size_t ss0)
{
    tss_entry->esp0 = esp0;
    tss_entry->ss0  = ss0;
}

// Initiate the TSS
void init_tss()
{
    memset((void*) tss_entry, 0, sizeof(tss_t));
    tss_set_stack(MM_KSTACK_TOP, GDT_KDATA_SEG);
    tss_flush();
}