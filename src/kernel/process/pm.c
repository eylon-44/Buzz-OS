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
#include <kernel/memory/gdt.h>
#include <kernel/interrupts/isr.h>
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

// Get the PID of the currently active process
inline int pm_get_pid() {
    return sched_get_active()->pid;
}

// Create a new process
process_t* pm_load(process_t* parent, uint32_t disk_offset, int priority)
{
    /* Create a new page directory for the process */
    pde_t *new_pd_p, *new_pd_v;
    {
        pde_t *pd_v;

        new_pd_p = (pde_t*) pmm_get_page();                         // allocate a physical page to hold the page directory of the new process
        new_pd_v = (pde_t*) vmm_attach_page((paddr_t) new_pd_p);    // temporarily map that page so we can access it
        pd_v = (pde_t*) vmm_attach_page((paddr_t) vmm_get_pd());    // map the current page directory as well so we can access it

        memset(new_pd_v, 0, MM_PAGE_SIZE);                          // initiate the new page directory with zeros

        memcpy(new_pd_v + MM_PDE_INDEX(MM_KSPACE_START),            // copy the common kernel space mapping into the new page directory
            pd_v + MM_PDE_INDEX(MM_KSPACE_START),
            (MM_PDE_INDEX(MM_CTX_DIFF_START) - MM_PDE_INDEX(MM_KSPACE_START)) * sizeof(pde_t));

        vmm_detach_page((vaddr_t) pd_v);                            // free temporarly attached page
    }


    /* Load and map the process into memory */
    elfheader_t elfhdr;
    {
        /* Note: offset variables ending with _d count sectors - offset within the disk, while offset
            variable ending with _f count bytes - offset within the file. */

        paddr_t scratch_p; vaddr_t scratch_v;
        size_t filesz;
        paddr_t* elfload_p;

        // Allocate and map a scratch space to load the ELF header into
        scratch_p = pmm_get_page();                 // physical address of scratch space
        scratch_v = vmm_attach_page(scratch_p);     // virtual address of scratch space

        // Load the first sector of the ELF file into the scratch space and read the elfheader
        pata_read_disk((void*) scratch_v, 1, disk_offset);

        // The ELF header is located at the start of the ELF file
        elfhdr = *(elfheader_t*) scratch_v;

        // Free scratch space
        pmm_free_page(scratch_p);
        vmm_detach_page(scratch_v);

        // Check ELF validity; if not valid, abort
        if (elfhdr.identify.magic != ELF_MAGIC      // ELF magic
            || elfhdr.identify.bitness != 1         // 32 bit executable
            || elfhdr.identify.abi != 0             // System-V
            || elfhdr.machine != 3)                 // x86
        {
            // [TODO] free PD
            return NULL;   // [TODO] ERRNUM
        }

        // Calculate the size of the ELF file; [start of section headers] + ([section header size] * [number of section headers])
        filesz = elfhdr.shoff + (elfhdr.shsize * elfhdr.shnum);

        // Allocate a list of pointers to save the physical pages to which we are going to load the ELF file into
        elfload_p = (paddr_t*) kmalloc(sizeof(paddr_t) * (MM_ALIGN_UP(filesz)/MM_PAGE_SIZE));

        // Load the entire file to memory and save the physical pages to the [elfload_p] list
        for (size_t i = 0; i < MM_ALIGN_UP(filesz)/MM_PAGE_SIZE; i++)
        {
            // Allocate a physical page and map it so we can access it
            elfload_p[i] = pmm_get_page();
            scratch_v    = vmm_attach_page(elfload_p[i]);

            // Read disk into the new page
            pata_read_disk((void*) scratch_v, MM_PAGE_SIZE/PATA_SECTOR_SIZE, disk_offset + i*(MM_PAGE_SIZE/PATA_SECTOR_SIZE));

            // Free the temporarly attached page
            vmm_detach_page(scratch_v);
        }

        /* Read all program headers and load their segments while mapping the user space for them.
            Assuming all program headers are located in the first 4KB of the file */
        for (size_t i = 0; i < elfhdr.phnum; i++) {
            prgheader_t prghdr;

            // Attach the first page of file, read program header and free
            scratch_v = vmm_attach_page(elfload_p[0]);
            prghdr = *(prgheader_t*) (scratch_v + (elfhdr.phoff + elfhdr.phsize*i)); 
            vmm_detach_page(scratch_v);

            // If the segment is not a LOAD segment, skip it
            if (prghdr.seg_type != 1) continue;

            size_t seg_start;          // offset in file to start loading the segment's data from
            size_t seg_end;            // offset in file to the end of the segment's data
            size_t file_pos;           // current offset in file
            paddr_t dest_p;            // segment's destination physical page
            vaddr_t dest_v;            // segment's destination virtual page
            size_t copy_size;          // how many bytes to copy
            size_t copy_dest;           // virtual address to load the data into

            seg_start = prghdr.offset;
            seg_end   = seg_start + prghdr.memsz;
            file_pos  = seg_start;
            copy_dest = prghdr.vaddr;

            // Load the segment
            while (seg_end - file_pos > 0)
            {
                /* Calculate the size needed to copy from the segment into memory */
                // If the data from [file_pos] to [seg_end] does not fit in the same page
                if (seg_end / MM_PAGE_SIZE != file_pos / MM_PAGE_SIZE) {
                    copy_size = MM_ALIGN_UP(file_pos) - file_pos;
                }
                // If it does fit in the same page
                else {
                    copy_size = seg_end - file_pos;
                }


                /* Map the user space for the segment */   
                // If the segment's page is mapped already, get the physical page of it
                if ( vmm_is_mapped(new_pd_p, MM_ALIGN_DOWN(copy_dest)) ) {
                    dest_p = vmm_get_physical(new_pd_p, MM_ALIGN_DOWN(copy_dest));
                }
                // If it is not, allocate a physical page for it and map it in the page directory of the new process
                else {
                    dest_p = pmm_get_page();

                    vmm_map_page(new_pd_p, dest_p,
                        MM_ALIGN_DOWN(copy_dest), prghdr.flags & 2,    // writable
                        1, 0, 0);
                }
                // Temporarily attach the user physical page so we can access it
                dest_v = vmm_attach_page(dest_p);// + (copy_dest % MM_PAGE_SIZE);


                /* Copy the segment's data into its destination */
                // Attach the file page so we can read from it
                scratch_v = vmm_attach_page(elfload_p[file_pos / MM_PAGE_SIZE]);

                // Data only
                if (seg_end - (file_pos+copy_size) >= prghdr.memsz - prghdr.filesz) {
                    memcpy((void*) dest_v, (void*) (scratch_v + (file_pos%MM_PAGE_SIZE)), copy_size);
                }
                // [TODO] finish BSS AND DATA&BSS
                // BSS only
                else if (seg_end - file_pos < prghdr.memsz - prghdr.filesz) {
                    memset((void*) dest_v, 0, copy_size);
                }
                // Data & BSS
                else {
                    // Copy data
                    // memcpy((void*) dest_v, (void*) (elfload_v + (file_start-file_pos)), prghdr.filesz);

                    // // Zero out BSS
                    // memset((void*) (dest_v + prghdr.filesz), 0, copy_size - prghdr.filesz);
                }

                // Seek the file
                copy_dest += copy_size;
                file_pos  += copy_size;

                // Free temporarly attached page
                vmm_detach_page(scratch_v);
                vmm_detach_page(dest_v);
            }
        }

        // Free resources
        kfree(elfload_p);
    }

    /* Queue the process in the scheduler */

    // Create the process data structure
    process_t process = { 
        .pid        = gen_pid(),                        // find an available PID for the process and set it
        .parnet     = parent,                           // parent of this process
        .child_count= 0,                                // 0 children by defualt
        .kesp       = 0,                                // kernel stack pointer to set when entering ring 0
        .cr3        = (uint32_t) new_pd_p,              // physical address of the process' page directory
        .status     = TS_NEW,                           // status of the task; will be set to READY once placed into the scheduler's queue
        .ticks      = 0,                                // ACTIVE time left; set by the scheduler
        .priority   = priority,                         // task's priority
        .entry      = (void (*)()) elfhdr.entry,        // process' entry function
        .exit_status= 0,                                // task's exit status
    };

    // Increment the child count of the parent
    if (parent != NULL) {
        parent->child_count++;
    }

    /* Allocate and set up kernel and user stacks for the new process.
    The interrupt frame will affect the initial values of the common register
    in the user space. These registers are used to pass argc and argc data.
    The IRET frame will affect the user entry point (EIP) and initial stack
    pointer (ESP).
    KESP = MM_KSTACK_TOP - sizeof(int_frame_t) - sizeof(iret_frame_t). */
    {
        size_t kesp;
        int_frame_t int_frame = {
            .eax=0xBABABABA,
            .ebx=0xCAFECAFE,
            .ecx=0xBEEFBEEF,
            .edx=0xDEAFDEAF,
            .esi=0xDEADDEAD,
            .ebp=0xBABEBABE,
            .error_code=0xABCDEFFF,
            .esp=MM_KSTACK_TOP - sizeof(int_frame_t)
        };
        iret_frame_t iret_frame = {
            .cs=GDT_UCODE_SEG,
            .eip=(size_t) process.entry,
            .ss=GDT_UDATA_SEG,
            .esp=MM_USTACK_TOP,
            .eflags=1<<9 /*IF*/ };

        // Allocate and map a page for the kernel stack of the new process
        kesp = pmm_get_page();
        vmm_map_page(new_pd_p, (paddr_t) kesp,
            MM_ALIGN_DOWN(MM_KSTACK_TOP), 1, 0, 0, 0);

        // Temporarly attach the stack so we can access it
        kesp = vmm_attach_page(kesp) + (MM_KSTACK_TOP % MM_PAGE_SIZE);

        // Place the frames in the stack
        kesp -= sizeof(iret_frame_t);
        *(iret_frame_t*) kesp = iret_frame;
        kesp -= sizeof(int_frame_t);
        *(int_frame_t*) kesp = int_frame;

        // Preserve the stack pointer as the initial kernel stack pointer of the process
        process.kesp = MM_KSTACK_TOP - sizeof(int_frame_t) - sizeof(iret_frame_t);

        // Allocate a user stack
        vmm_map_page(new_pd_p,
            pmm_get_page(),
            MM_ALIGN_DOWN(MM_USTACK_TOP),
            1, 1, 0, 0);

        // Free temporarily attached page
        vmm_detach_page((vaddr_t) kesp);
    }

    /* Map the last 4MB of self refrenced paging */
    for (size_t i = 0; i < MM_PD_ENTRIES; i++) {
        if (!new_pd_v[i].present) continue;                 // skip if there is no page table for this area
        vmm_map_page(new_pd_p,
            (paddr_t) MM_GET_PT(new_pd_v+i),                // physical address of page table
            (vaddr_t) MM_PTD_START + (MM_PAGE_SIZE * i),    // virtual address of page table
                1, 0, 0, 0);
    }

    // Free temporarily attached page
    vmm_detach_page((vaddr_t) new_pd_v);

    // Queue the task in the scheduler's queue and return it
    return sched_add_process(process);
}

/* Kill a process.
    Set its status to DONE and let the scheduler handle the rest. */
void pm_kill(process_t* t)
{
    sched_set_status(t, TS_DONE);
    if (t == sched_get_active()) {
        sched_switch_next();
    }
}

// Initiate the initiation process
static void init_init()
{
    process_t* init_p;              // init process
    extern process_t _dummy_proc;   // extern dummy process created by the virtual memory manager
    extern sched_queue_t queue;     // extern scheduler queue to manipulate it

    /* Add the dummy process to the queue and set its status to DONE.
        The dummy process represents the process of the current startup context,
        that holds no user process, and therefore should be deleted after initialization. */
    sched_set_status( sched_add_process(_dummy_proc), TS_DONE );
    queue.active = queue.proc_list;

    // Load the init process
    init_p = pm_load(NULL, 512, 20); // DEBUG SECTOR NUMBER
    pm_load(init_p, 512, 10);   // DEBUG
}

void init_pm()
{
    init_tss();
    init_scheduler();
    init_init();
}