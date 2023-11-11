// Paging Hanlder // ~ eylon

#include <kernel/memory/paging.h>

// Extern assembly function to enable paging
extern void asm_enable_paging();

pde_t page_directory[PAGE_TABLE_ENTRIES] __attribute__((aligned(PAGE_SIZE))) = {
    [0]                PDE_PS4MB | PTE_PRESENT | PTE_WRITE_ACCESS,
    [0xC0000000 >> 22] PDE_PS4MB | PTE_PRESENT | PTE_WRITE_ACCESS
};

void init_paging()
{
    //identity_paging();
    //asm_enable_paging();
}