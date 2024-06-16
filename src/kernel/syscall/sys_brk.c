// sys_brk // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>

/* Change data segment size.
    
    brk() changes the location of the program break, which defines the end of
    the process's data segment (the location of the _end symbol created by the
    linker). Increasing the program break has the effect of allocating memory to
    the process; decreasing the break deallocates memory.

    brk() sets the end of the data segment to the value specified by [addr], when
    that value is reasonable, the system has enough memory, and the process does
    not exceed its maximum data size.

    On success, brk() returns zero. On error, -1 is returned.

    int syscall(SYS_brk, void* addr);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_brk(int_frame_t* param)
{
    if (pm_brk(pm_get_active(), param->ebx)) {
        param->eax = 0;
    }
    else {
        param->eax = -1;
    }
}