// sys_sbrk // ~ eylon

#include <kernel/syscall.h>
#include <kernel/process/pm.h>
#include <libc/stdint.h>

/* Change data segment size.
    
    sbrk() changes the location of the program break, which defines the end of
    the process's data segment (the location of the _end symbol created by the
    linker). Increasing the program break has the effect of allocating memory to
    the process; decreasing the break deallocates memory.

    sbrk() increments the program's data space by [increment] bytes. Calling sbrk()
    with [increment] set to 0 can be used to find the current location of the program break.

    On success, sbrk() returns the new program break. On error, NULL is returned.
           
    void* syscall(SYS_sbrk, size_t increment);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_sbrk(int_frame_t* param)
{
    param->eax = pm_brk(pm_get_active(), pm_get_active()->pbrk + param->ebx);
    if (!param->eax) param->eax = (uint32_t) NULL;
}