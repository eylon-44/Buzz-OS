// sys_chdir // ~ eylon

#include <kernel/syscall.h>
#include <kernel/fs.h>
#include <kernel/process/pm.h>
#include <libc/sys/stat.h>

/* Change working directory.
    
    The chdir() syscall shall cause the directory named by the
    pathname pointed to by the [path] argument to become the current
    working directory; that is, the starting point for path searches
    for pathnames not beginning with '/'.

    Upon successful completion, 0 shall be returned. Otherwise, -1 shall be
    returned and the current working directory shall remain unchanged.

    int syscall(SYS_chdir, const char* path);
    param: [0] EAX, [1] EBX, [2] ECX, [3] EDX, [4] ESI, [5] EDI
*/
void sys_chdir(int_frame_t* param)
{
    struct stat fstat;
    if (fs_stat((const char*) param->ebx, &fstat) != 0 || fstat.type != DT_DIR) {
        param->eax = -1;
        return;
    }

    pm_get_active()->cwd = fstat.indx;
    param->eax = 0;
}