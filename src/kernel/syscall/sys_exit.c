// sys_exit // ~ eylon

#include <kernel/syscall.h>

/* Terminate the calling process.
*/

void sys_exit(int_frame_t* param)
{
    param->eax = 1289;
}