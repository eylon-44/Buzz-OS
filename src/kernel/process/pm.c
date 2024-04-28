// Process Manager // ~ eylon

#include <kernel/process/pm.h>
#include <kernel/process/scheduler.h>
#include <kernel/process/tss.h>
#include <kernel/process/elf.h>
#include <kernel/memory/mm.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmlayout.h>
#include <drivers/pata.h>
#include <libc/stdint.h>
#include <libc/string.h>

/* Generate a unique process ID.
    The function simply assumes that we will never create more than 4 billion (2^32) processes.
    So instead of iterating over the list of processes in order to make sure that there are no
    duplicates (which can take a lot of time), we simply increament a value by 1 and return it. */
static inline int gen_pid()
{
    static int pid = -1;
    pid++;
    return pid;
}

/* Generate a unique thread ID.
    The function simply assumes that we will never create more than 4 billion (2^32) threads.
    So instead of iterating over the list of threads in order to make sure that there are no
    duplicates (which can take a lot of time), we simply increament a value by 1 and return it. */
static inline int gen_tid()
{
    static int tid = -1;
    tid++;
    return tid;
}

// Get the PID of the currently active process
int pm_get_pid() {
    return 0;
}
/* I am not sure how to store process ids. One way to have for each thread an int PID (TGID) and an int TID.
    Another is to have a struct or just a single int to hold both. */

thread_t* pm_load(thread_t* parent, uint32_t disk_offset, int priority)
{
    /* Merge the kernel thread with the calling thread */
    __asm__ volatile ("sti");                                   // enable interrupts


    /* Create a new page directory for the process */
    pde_t *new_pd_p, *new_pd_v, *pd_v;

    new_pd_p = (pde_t*) pmm_get_page();                         // allocate a physical page to hold the page directory of the new process
    new_pd_v = (pde_t*) vmm_attach_page((paddr_t) new_pd_p);    // temporarily map that page so we can access it
    pd_v = (pde_t*) vmm_attach_page((paddr_t) vmm_get_pd());    // map the current page directory as well so we can access it

    memset(new_pd_v, 0, MM_PAGE_SIZE);                          // initiate the new page directory with zeros
    memcpy(new_pd_v + MM_PD_ENTRIES*(3/4),                      // copy the kernel space mapping into the new page directory
        pd_v + MM_PD_ENTRIES*(3/4),
        MM_PAGE_SIZE*(1/4));

    vmm_detach_page((vaddr_t) pd_v);                            // free the temporarily attached page

    vmm_map_page(new_pd_v, pmm_get_page(),                      // allocate and map a kernel stack for the new process
        MM_ALIGN_DOWN(MM_KSTACK_TOP), 1, 0, 0, 0);
    /* !!! I am pretty sure that the last 4mb of self refrenced mapping isn't correct after a copy because 
        it includes pages from the user space. FIX THIS!!! */


    /* Load and map the process into memory */
    paddr_t elfload_p; vaddr_t elfload_v;
    elfheader_t elfhdr;
    size_t filesz UNUSED;

    // Allocate and map a scratch space to load the ELF file into
    elfload_p = pmm_get_page();
    elfload_v = vmm_attach_page(elfload_p);

    // Load the first page of the ELF file into a scratch space and read the elfheader
    pata_read_disk((void*) elfload_v, MM_PAGE_SIZE, disk_offset);

    // The ELF header is located at the start of the ELF file
    elfhdr = *(elfheader_t*) elfload_v;

    // Check ELF validity by reading it's header; if not valid, abort
    if (elfhdr.identify.magic != ELF_MAGIC      // ELF magic
        || elfhdr.identify.bitness != 1         // 32 bit executable
        || elfhdr.identify.abi != 0             // System-V
        || elfhdr.machine != 3)                 // x86
    {
        // [TODO] free resources
        return NULL;   // [TODO] ERRNUM
    }

    // Calculate the size of the ELF file; [start of section headers] + ([section header size] * [number of section headers])
    filesz = elfhdr.shoff + (elfhdr.shsize * elfhdr.shnum);
    
    // Read and load all segments
    for (size_t i = 0; i < elfhdr.phnum; i++)
    {
        size_t ph_offset, load_offset;
        prgheader_t prgheader;
        size_t start_offset, copy_size, end_offset, copy_dest;

        // Offset of the program header inside the file
        ph_offset = elfhdr.phoff + elfhdr.phsize * i;
        // Offset in file to load into [elfload_v]
        load_offset = MM_ALIGN_X_DOWN(ph_offset, PATA_SECTOR_SIZE);
        // Load 2 sectors from offset [load_offset] in the file into [elfload_v]
        pata_read_disk((void*) elfload_v, PATA_SECTOR_SIZE * 2, disk_offset + load_offset);
        // Read the program header
        prgheader = *(prgheader_t*) (elfload_v + ph_offset - load_offset);

        /* If it's not a LOAD segment (seg_type=1), continue; else, load and map it into the process. */
        if (prgheader.seg_type != 1) continue;

        start_offset    = prgheader.offset;             // segment start offset in file
        copy_size       = prgheader.filesz;             // number of bytes to copy from the start offset
        end_offset      = start_offset + copy_size;     // segment end offset in file
        copy_dest       = prgheader.vaddr;              // virtual address to load the data into
        load_offset     = MM_ALIGN_DOWN(start_offset);  // the file offset loaded as the base of the page

        // While not finished loading the segment from start to end
        while(end_offset - start_offset > 0)
        {
            // Read 4KB from offset [load_offset] in the file into the scratch space
            pata_read_disk((void*) elfload_v, MM_PAGE_SIZE, disk_offset + load_offset);

            /* Calculate the size needed to copy from the loaded segment into memory */
            // If the segment does not fit fully in the loaded page
            if (end_offset - load_offset > MM_PAGE_SIZE) {
                copy_size = load_offset + MM_PAGE_SIZE - start_offset;
            }
            // If it does fit fully in the loaded page
            else {
                copy_size = end_offset - start_offset;
            }

            // If the segment's page is not mapped already, allocate and map it
            if (!vmm_is_mapped(new_pd_p, MM_ALIGN_DOWN(copy_dest))) {
                vmm_map_page(new_pd_p, pmm_get_page(),
                    MM_ALIGN_DOWN(copy_dest), prgheader.flags & 2,    // writable
                    1, 0, 0);
            }

            /* Copy the segment's data into its destination */
            // Data only
            if (copy_size <= prgheader.filesz) {
                memcpy((void*) copy_dest, (void*) (elfload_v + (start_offset-load_offset)), copy_size);
            }
            // BSS only
            else if (prgheader.filesz <= 0) {
                memset((void*) copy_dest, 0, copy_size);
            }
            // Data & BSS
            else {
                // Copy data
                memcpy((void*) copy_dest, (void*) (elfload_v + (start_offset-load_offset)), prgheader.filesz);

                // Zero out BSS
                memset((void*) (copy_dest + prgheader.filesz), 0, copy_size - prgheader.filesz);
            }

            prgheader.filesz -= copy_size;
            prgheader.memsz  -= copy_size;
            load_offset     += MM_PAGE_SIZE;
            start_offset    += copy_size;
            copy_dest       += copy_size;
        }
    }

    // Free resources
    pmm_free_page(elfload_p);
    vmm_detach_page(elfload_v);


    /* Queue the process in the scheduler */

    // Create the thread data structure
    thread_t thread = { 
        .pid        = gen_pid(),                        // find an available PID for the process and set it
        .tid        = gen_tid(),                        // find an available TID for and set it
        .parnet     = parent,                           // parent of this process; NULL for init process
        .child_count= 0,                                // 0 children by defualt
        .kesp       = MM_KSTACK_TOP,                    // kernel stack top to set when entering ring 0
        .cr3        = (uint32_t) new_pd_p,              // physical address of the process' page directory
        .status     = TS_NEW,                           // status of the task; will be set to READY once placed into the scheduler's queue
        .ticks      = 0,                                // ACTIVE time left; set by the scheduler
        .priority   = priority,                         // task's priority
        .entry      = (void (*)()) elfhdr.entry,        // process' entry function
        .exit_status= 0                                 // task's exit status
    };

    // Increment the child count of the parent
    parent->child_count++;

    // Queue the task in the scheduler's queue and return it
    return sched_add_thread(thread);
}

// Initiate the initiation process
static void init_init()
{
    /* As all of the function are implemented with the assumption that there is already at least one
        process in the queue (which should be the init process), we will have to manipulate them by
        creating a dummy process. That way we could use these function in order to create the actual
        first process - the init process. */

    thread_t* init_t;                       // init thread
    thread_node_t dummy = { .next=NULL };   // dummy process
    extern squeue_t queue;                  // extern the scheduler's queue to manipulate it for the init process

    // Set the dummy process
    queue.active = &dummy;

    // Load the process into memory
    init_t = pm_load(NULL, 0, 20); // ??? [TODO]

    /* The [pm_load] function uses the [sched_add_thread] function which places our [init_t]
        thread at the end of the queue - right after the dummy process. We can now delete the
        dummy process and replace it with the init process. */
    queue.thread_n = queue.active->next;
    queue.active   = queue.thread_n;

    // Switch to the init process
    sched_switch(init_t);
}

void init_pm()
{
    init_tss();
    init_scheduler();
    init_init();
}