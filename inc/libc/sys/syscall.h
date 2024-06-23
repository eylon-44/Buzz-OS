// sys/syscall.h // ~ eylon

#if !defined(__LIBC_SYSCALL_H)
#define __LIBC_SYSCALL_H

#define SYS_nop         0x00
#define SYS_exit        0x01
#define SYS_read        0x03
#define SYS_write       0x04
#define SYS_open        0x05
#define SYS_close       0x06
#define SYS_creat       0x08
#define SYS_link        0x09
#define SYS_unlink      0x0a
#define SYS_execve      0x0b
#define SYS_chdir       0x0c
#define SYS_militime    0x0d
#define SYS_lseek       0x13
#define SYS_brk         0x2d
#define SYS_sbrk        0x5a
#define SYS_sched_yield	0x9e
#define SYS_milisleep   0xa2
#define SYS_mkdir       0x27
#define SYS_truncate    0x5c
#define SYS_ftruncate   0x5d
#define SYS_stat        0x12
#define SYS_fstat       0x1c
#define SYS_rmdir       0x28
#define SYS_getcwd      0xb7
#define SYS_rename      0x26
#define SYS_ps          0xe5
#define SYS_pr          0xaf

#define SYS_int 92      // syscall interrupt number (0x5C)

#endif