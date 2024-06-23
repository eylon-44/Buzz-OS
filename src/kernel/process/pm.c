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
#include <kernel/fs.h>
#include <kernel/memory/gdt.h>
#include <kernel/interrupts/isr.h>
#include <drivers/pata.h>
#include <libc/stdint.h>
#include <libc/string.h>
#include <libc/unistd.h>
#include <libc/fcntl.h>
#include <libc/limits.h>
#include <libc/unistd.h>
#include <libc/sys/syscall.h>

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
// Get a pointer to the active process
inline process_t* pm_get_active() {
    return sched_get_active();
}

/* Create a new process.
    [path] must be a file path for an ELF file.
    On success returns a pointer to the new process structure, on failure returns NULL. */
process_t* pm_load(process_t* parent, const char* path, UNUSED char* const argv[], int priority)
{
    int fd;
    elfheader_t elfhdr;
    pde_t *new_pd_p;
    vaddr_t scratch_v; paddr_t scratch_p;
    process_t process;
    process_t* process_ptr;
    size_t pbrk = 0;

    // Open the ELF file
    fd = fs_open(path, O_RDONLY);
    if (fd < 0) {
        return NULL;
    }

    // Read the ELF header and check its validity
    fs_read(fd, &elfhdr, sizeof(elfheader_t));
    if (elfhdr.identify.magic != ELF_MAGIC      // ELF magic
        || elfhdr.identify.bitness != 1         // 32 bit executable
        || elfhdr.identify.abi != 0             // System-V
        || elfhdr.machine != 3)                 // x86
    {
        return NULL;
    }

    /* Create a page directory for the new process */
    {
        pde_t *pd_v;

        new_pd_p = (pde_t*) pmm_get_page();                         // allocate a physical page to hold the page directory of the new process
        scratch_v = vmm_attach_page((paddr_t) new_pd_p);            // temporarily map that page so we can access it
        pd_v = (pde_t*) vmm_attach_page((paddr_t) vmm_get_pd());    // map the current page directory as well so we can access it

        memset((void*) scratch_v, 0, MM_PAGE_SIZE);                 // initiate the new page directory with zeros

        memcpy((void*) ((pde_t*) scratch_v + MM_PDE_INDEX(MM_KSPACE_START)), // copy the common kernel space mapping into the new page directory
            pd_v + MM_PDE_INDEX(MM_KSPACE_START),
            (MM_PDE_INDEX(MM_CTX_DIFF_START) - MM_PDE_INDEX(MM_KSPACE_START)) * sizeof(pde_t));

        // Free temporarly attached pages
        vmm_detach_page((vaddr_t) pd_v);
        vmm_detach_page(scratch_v);                           
    }

    /* Load and map the new process into memory.
        Read all program headers and load their segments while mapping them into the user space. */
    for (size_t i = 0; i < elfhdr.phnum; i++)
    {
        prgheader_t prghdr;

        // Read the program header at index [i]
        fs_lseek(fd, elfhdr.phoff + elfhdr.phsize*i, SEEK_SET);
        fs_read(fd, &prghdr, sizeof(prgheader_t));

        // If the segment is not a LOAD segment, skip it
        if (prghdr.seg_type != 1) continue;

        // Get the program break
        if (prghdr.vaddr+prghdr.memsz > pbrk) pbrk = MM_ALIGN_UP(prghdr.vaddr+prghdr.memsz);

        // Seek the start of the data
        fs_lseek(fd, prghdr.offset, SEEK_SET);

        // Load and map the segment
        for (size_t load_addr = prghdr.vaddr; load_addr < prghdr.vaddr + prghdr.memsz;)
        {
            size_t load_size;
            
            // If page is mapped already, attach its physical page
            if (vmm_is_mapped(new_pd_p, load_addr)) {
                scratch_v = vmm_attach_page(vmm_get_physical(new_pd_p, load_addr));
            }
            // Else, allocate and map it
            else {
                scratch_p = pmm_get_page();
                scratch_v = vmm_attach_page(scratch_p);
                // vmm_map_page(new_pd_p, scratch_p, load_addr, prghdr.flags & ~2, 1, 0, 0);
                vmm_map_page(new_pd_p, scratch_p, load_addr, (prghdr.flags >> 2) & 1, 1, 0, 0);
            }

            // Load data from file
            if (load_addr < prghdr.vaddr + prghdr.filesz)
            {
                // Calculate the number of bytes to load
                load_size = MM_PAGE_SIZE - (load_addr % MM_PAGE_SIZE);
                if (load_addr + load_size > prghdr.vaddr + prghdr.filesz) {
                    load_size = prghdr.vaddr + prghdr.filesz - load_addr;
                }
                // Read [load_size] bytes from file into the scratch space
                fs_read(fd, (void*) (scratch_v + (load_addr % MM_PAGE_SIZE)), load_size);
                load_addr += load_size;
            }
            // Zero out BSS section
            if (load_addr >= prghdr.vaddr + prghdr.filesz && prghdr.filesz < prghdr.memsz)
            {
                load_size = MM_PAGE_SIZE - (load_addr % MM_PAGE_SIZE);
                if (load_addr + load_size > prghdr.vaddr + prghdr.memsz) {
                    load_size = prghdr.vaddr + prghdr.memsz - load_addr;
                }
                // Set [load_size] bytes to 0 at the scratch space
                memset((void*) (scratch_v + (load_addr % MM_PAGE_SIZE)), 0, load_size);
                load_addr += load_size;
            }

            // Free temporarly attached page
            vmm_detach_page(scratch_v);
        }
    }

    /* Queue the process in the scheduler */
    // Create the process data structure
    process = (process_t) {
        .pid        = gen_pid(),                    // find an available PID for the process and set it
        .parent     = parent,                       // parent of this process
        .child_count= 0,                            // 0 children by defualt
        .kesp       = 0,                            // kernel stack pointer to set when entering ring 0
        .cr3        = (uint32_t) new_pd_p,          // physical address of the process' page directory
        .tab        = NULL,                         // UI tab
        .status     = PSTATUS_NEW,                  // status of the task; will be set to READY once placed into the scheduler's queue
        .pbrk       = pbrk,                         // program break
        .ticks      = 0,                            // ACTIVE time left; set by the scheduler
        .priority   = priority,                     // task's priority
        .fds        = NULL,                         // process's open file descriptors list
        .entry      = (void (*)()) elfhdr.entry,    // process' entry function
        .exit_status= 0,                            // task's exit status
    };

    // Close the ELF file
    fs_close(fd);

    // If new process has a parent
    if (parent != NULL) {
        parent->child_count++;
        process.tab = parent->tab;
        process.cwd = parent->cwd;
        sched_set_status(parent, PSTATUS_BLOCKED);
    }
    else {
        process.cwd = fs_seek(PM_DEFUALT_CWD);
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
            .eax=0,
            .ebx=0,
            .ecx=0,
            .edx=0,
            .esi=0,
            .ebp=0,
            .error_code=0xABCDEFFF,
            .esp=MM_KSTACK_TOP - sizeof(int_frame_t)
        };
        iret_frame_t iret_frame = {
            .cs=GDT_UCODE_SEG,
            .eip=(size_t) process.entry,
            .ss=GDT_UDATA_SEG,
            .esp=MM_USTACK_TOP - 8,
            .eflags=1<<9 /*IF*/ };

        // Allocate and map a page for the kernel stack of the new process
        kesp = pmm_get_page();
        vmm_map_page(new_pd_p, (paddr_t) kesp,
            MM_ALIGN_DOWN(MM_KSTACK_TOP), 1, 0, 0, 0);

        // Preserve the stack pointer as the initial kernel stack pointer of the process
        process.kesp = MM_KSTACK_TOP - sizeof(int_frame_t) - sizeof(iret_frame_t);


        // Allocate a user stack and set it up with argc & argv
        scratch_p = pmm_get_page();
        if (argv != NULL)
        {
            char* argv_ptr[ARGC_MAX];
            size_t arg_i;

            // Attach the user stack
            scratch_v = vmm_attach_page(scratch_p);

            // Copy the [argv] string into the stack
            for (arg_i = 0; argv[arg_i] != NULL && arg_i < ARGC_MAX; arg_i++)
            {
                size_t len = MM_ALIGN_X_UP(strlen(argv[arg_i])+1, 4);           // get the length of the string
                iret_frame.esp -= len;                                          // allocate space on the stack for the string
                memcpy((void*) (scratch_v + iret_frame.esp % MM_PAGE_SIZE),     // copy the string into the stack
                    argv[arg_i], len);
                argv_ptr[arg_i] = (char*) iret_frame.esp;                       // save a pointer to the string on the stack
            }

            // Copy the string pointers and count into the stack
            iret_frame.esp -= sizeof(argv_ptr) * arg_i;                         // allocate [arg_i] string pointers on the stack
            memcpy((void*) (scratch_v + iret_frame.esp % MM_PAGE_SIZE),         // copy the string pointers into the stack
                (void*) argv_ptr, sizeof(argv_ptr) * arg_i);
            iret_frame.esp -= sizeof(arg_i);

            *(size_t*) (scratch_v + iret_frame.esp % MM_PAGE_SIZE) = arg_i;
            vmm_detach_page(scratch_v);
        }
        // Map the user stack
        vmm_map_page(new_pd_p,
            scratch_p,
            MM_ALIGN_DOWN(MM_USTACK_TOP),
            1, 1, 0, 0);


        // Temporarly attach the kernel stack so we can access it
        kesp = vmm_attach_page(kesp) + (MM_KSTACK_TOP % MM_PAGE_SIZE);

        // Place the frames in the stack
        kesp -= sizeof(iret_frame_t);
        *(iret_frame_t*) kesp = iret_frame;
        kesp -= sizeof(int_frame_t);
        *(int_frame_t*) kesp = int_frame;

        // Free temporarily attached page
        vmm_detach_page((vaddr_t) kesp);
    }

    /* Map the last 4MB of self refrenced paging */
    scratch_v = vmm_attach_page((paddr_t) new_pd_p);
    for (size_t i = 0; i < MM_PD_ENTRIES; i++) {
        if (!((pde_t*) scratch_v)[i].present) continue;                // skip if there is no page table for this area
        vmm_map_page(new_pd_p,
            (paddr_t) MM_GET_PT(((pde_t*) scratch_v)+i),               // physical address of page table
            (vaddr_t) MM_PTD_START + (MM_PAGE_SIZE * i),    // virtual address of page table
                1, 0, 0, 0);
    }
    vmm_detach_page((vaddr_t) scratch_v);

    // Queue the new process in the scheduler
    process_ptr = sched_add_process(process);
    // If parent should be blocked, yield
    if (parent != NULL && parent->status == PSTATUS_BLOCKED) {
        syscall(SYS_sched_yield);   // see you later!
    }

    // Queue the task in the scheduler's queue and return it
    return process_ptr;
}

/* Kill a process.
    Set its status to DONE and let the scheduler handle the rest. */
void pm_kill(process_t* proc)
{
    // Set process and all of his children to done
    sched_set_status(proc, PSTATUS_DONE);
    if (proc->child_count > 0)
    {
        extern sched_queue_t queue;
        process_t* child = queue.proc_list;
        while (child != NULL) {
            if (child->parent == proc) {
                pm_kill(child);
            }
            child = child->next;
        }
    }

    // Unblock the parent
    if (proc->parent != NULL) {
        sched_set_status(proc->parent, PSTATUS_READY);
        proc->parent->child_count--;
    }
    // If process has no parent and a tab, close its tab
    else if (proc->tab != NULL) {
        ui_tab_close_tab(proc->tab);
    }

    // If the process is the active process switch it
    if (proc == sched_get_active()) {
        sched_switch_next();
    }
}

/* Set the program break of a given process.
    Set the program break of [proc] to [addr]. If [addr] is bigger than the current
    program break, allocate pages, else, deallocate pages. [addr] must be within the userspace.
    [addr] must be 4KB aligned; if not, [addr] will be rounded up.
    On success returns the new program break. On error returns 0. */
size_t pm_brk(process_t* proc, size_t addr)
{
    // Round [addr] up to get the program break top
    addr = MM_ALIGN_UP(addr);

    // If [addr] is not within the address space, return NULL
    if (addr > MM_KSPACE_START || addr < MM_USPACE_START) return 0;

    // If [addr] is bigger than the current program break, allocate pages
    for (; addr > proc->pbrk; proc->pbrk+=MM_PAGE_SIZE) {
        vmm_map_page((pde_t*) proc->cr3, pmm_get_page(), proc->pbrk, 1, 1, 0, 0);
    }
    // If [addr] is smaller than the current program break, deallocate pages
    for (; addr < proc->pbrk; proc->pbrk-=MM_PAGE_SIZE) {
        pmm_free_page(vmm_get_physical((pde_t*) proc->cr3, proc->pbrk-MM_PAGE_SIZE));
        vmm_unmap_page((pde_t*) proc->cr3, proc->pbrk-MM_PAGE_SIZE);
    }

    // Return the program break
    return proc->pbrk;
}

// Error handler
static void handle_error()
{
    // Kill the active process
    pm_kill(pm_get_active());
}

// Initiate the error handler 
static void init_error()
{
    // Register all error interrupts to the error handler function
    for (int i = 0; i < IRQ0; i++) {
        set_interrupt_handler(i, handle_error);
    }
}

// Terminate the dummy process
static void kill_dummy_proc()
{
    extern process_t _dummy_proc;   // extern dummy process created by the virtual memory manager
    extern sched_queue_t queue;     // extern scheduler queue to manipulate it

    /* Add the dummy process to the queue and set its status to DONE.
        The dummy process represents the process of the current startup context,
        that holds no user process, and therefore should be deleted after initialization. */
    sched_set_status(sched_add_process(_dummy_proc), PSTATUS_DONE);
    queue.active = queue.proc_list;

    // Load a busy process
    pm_load(NULL, "/sys/_busy.elf", NULL, 0);
}

void init_pm()
{
    init_tss();
    init_scheduler();
    kill_dummy_proc();
    init_error();
}