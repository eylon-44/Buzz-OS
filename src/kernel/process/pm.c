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
inline int pm_get_pid() {
    return sched_get_active()->pid;
}

// Create a new process
thread_t* pm_load(thread_t* parent, uint32_t disk_offset, int priority)
{
    /* Merge the kernel thread with the calling thread */
    //__asm__ volatile ("sti");                                   // enable interrupts


    /* Create a new page directory for the process */
    pde_t *new_pd_p, *new_pd_v, *pd_v;

    new_pd_p = (pde_t*) pmm_get_page();                         // allocate a physical page to hold the page directory of the new process
    new_pd_v = (pde_t*) vmm_attach_page((paddr_t) new_pd_p);    // temporarily map that page so we can access it
    pd_v = (pde_t*) vmm_attach_page((paddr_t) vmm_get_pd());    // map the current page directory as well so we can access it

    memset(new_pd_v, 0, MM_PAGE_SIZE);                          // initiate the new page directory with zeros
    
    memcpy(new_pd_v + MM_PD_ENTRIES * 3/4,                      // copy the kernel space mapping into the new page directory
        pd_v + MM_PD_ENTRIES * 3/4,
        MM_PD_ENTRIES*sizeof(pde_t) * 1/4);

    vmm_map_page(new_pd_v, pmm_get_page(),                      // allocate and map a kernel stack for the new process
        MM_ALIGN_DOWN(MM_KSTACK_TOP), 1, 0, 0, 0);

    // Remove the user space part (3/4 - until 3GB) of the last 4MB of self refrenced mapping
    for (size_t vbase = MM_PTD_START; vbase < MM_PTD_START + (MB(4) * 3/4); vbase += MM_PAGE_SIZE) {
        vmm_unmap_page(new_pd_p, vbase);
    }

    vmm_detach_page((vaddr_t) new_pd_v);                        // free the temporarily attached page that held the current page directory

    /* Load and map the process into memory */
    paddr_t elfload_p; vaddr_t elfload_v;
    elfheader_t elfhdr;
    size_t filesz UNUSED;

    // Allocate and map a scratch space to load the ELF file into
    elfload_p = pmm_get_page();                                 // physical address of scratch space
    elfload_v = vmm_attach_page(elfload_p);                     // virtual address of scratch space

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
        // Free resources and return
        pmm_free_page(elfload_p);
        vmm_detach_page(elfload_v);
        // [TODO] free: kstack, pd_p, pd_v
        return NULL;   // [TODO] ERRNUM
    }

    // Calculate the size of the ELF file; [start of section headers] + ([section header size] * [number of section headers])
    filesz = elfhdr.shoff + (elfhdr.shsize * elfhdr.shnum);
    
    // Read and load all segments
    for (size_t i = 0; i < elfhdr.phnum; i++)
    {
        size_t ph_offset;           // program header offset in the file
        size_t load_offset;         // offset in file to load into the scratch space (elfload_v)
        prgheader_t prgheader;      // program header of current segment
        size_t start_offset;        // offset in file to start loading the segment's data from
        size_t end_offset;          // offset in file to the end of the segment's data
        size_t copy_dest;           // virtual address to load the segment's data into in the user address space
        size_t copy_size;           // how many bytes to copy from the scratch space to the user address space
        paddr_t dest_p;             // copy destination physical page
        vaddr_t dest_v;             // copy destination virtual page

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

            // If the segment's page is mapped already, get the physical page of it
            if ( vmm_is_mapped(new_pd_p, MM_ALIGN_DOWN(copy_dest)) ) {
                dest_p = vmm_get_physical(new_pd_p, MM_ALIGN_DOWN(copy_dest));
            }
            // If it is not, allocate a physical page for it and map it in the page directory of the new process
            else {
                dest_p = pmm_get_page();

                vmm_map_page(new_pd_p, dest_p,
                    MM_ALIGN_DOWN(copy_dest), prgheader.flags & 2,    // writable
                    1, 0, 0);
            }
            // Temporarily attach the physical page so we can write to it; add the copy_dest offset to it
            dest_v = vmm_attach_page(dest_p) + (copy_dest % MM_PAGE_SIZE);

            /* Copy the segment's data into its destination */
            // Data only
            if (copy_size <= prgheader.filesz) {
                memcpy((void*) dest_v, (void*) (elfload_v + (start_offset-load_offset)), copy_size);
            }
            // BSS only
            else if (prgheader.filesz <= 0) {
                memset((void*) dest_v, 0, copy_size);
            }
            // Data & BSS
            else {
                // Copy data
                memcpy((void*) dest_v, (void*) (elfload_v + (start_offset-load_offset)), prgheader.filesz);

                // Zero out BSS
                memset((void*) (dest_v + prgheader.filesz), 0, copy_size - prgheader.filesz);
            }

            prgheader.filesz -= copy_size;
            prgheader.memsz  -= copy_size;
            load_offset     += MM_PAGE_SIZE;
            start_offset    += copy_size;
            copy_dest       += copy_size;
            vmm_detach_page(dest_v);
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
        .exit_status= 0,                                // task's exit status
    };

    // Increment the child count of the parent
    parent->child_count++;

    // Queue the task in the scheduler's queue and return it
    return sched_add_thread(thread);
}

/* Kill a process.
    Set its status to DONE and let the scheduler handle the rest. */
void pm_kill(thread_t* t)
{
    sched_set_status(t, TS_DONE);
    if (t == sched_get_active()) {
        sched_switch_next();
    }
}

// Initiate the initiation process
static void init_init()
{
    /* As all of the function are implemented with the assumption that there is already at least one
        process in the queue (which should be the init process), we will have to manipulate them by
        using the dummy process created by the VMM in init_vmm and already stored in the queue for us.
        By using this dummy we gain access to all process related functions that will help us to create
        the actual first process - the init process. The dummy represents the process of the current
        init context, that holds no user process, and therefore should be deleted after initiation. */

    thread_t* init_t;                                                       // init thread
    extern sched_queue_t queue;                                             // extern the scheduler's queue to manipulate it for the init process
    
    // Load the init process
    init_t = pm_load(&queue.active->thread, 512, 20); // DEBUG SECTOR NUMBER

    // Remove the dummy as being the parnet
    init_t->parnet = NULL;

    // Switch to the init process which should be located right after the dummy in the queue
    sched_switch_next();
}

void init_pm()
{
    init_tss();
    init_scheduler();
    init_init();
}