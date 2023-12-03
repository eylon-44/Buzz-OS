// Global Descriptor Table Header File // ~ eylon

#if !defined(GDT_H)
#define GDT_H

#include <utils/type.h>

// Segment Selectors
#define SELECTOR_KERNEL_CODE 0x08
#define SELECTOR_KERNEL_DATA 0x10
#define SELECTOR_USER_CODE   0x18
#define SELECTOR_USER_DATA   0x20
#define SELECTOR_TSS         0x28
#define SEG_COUNT            5

typedef struct 
{
    u8_t accessed:   1;         // being set by the cpu when the segment is accessed
    u8_t rw:         1;         /* [code seg] readable bit  :: <0>=read access is forbidden <1>=read access is allowed
                                   [data seg] writeable bit :: <0>=write access is forbidden <1>=write access is allowed */
    u8_t dc:         1;         /* [code seg] conforming bit
                                   [data seg] direction bit :: <0>=grow up <1>=grow down */
    u8_t executable: 1;         // executable               :: <0>=data segment <1>=code segment
    u8_t type:       1;         // descriptor type          :: <0>=system segment (TSS) <1>=code/data segment
    u8_t dpl:        2;         // descriptor privilege level
    u8_t present:    1;         // present                  :: <0>=invalid segment <1>=valid segment
} __attribute__ ((packed)) gdt_access_t;

typedef enum 
{
    GDT_FLAG_LONG        0b0010,      // use a long mode code segment, there will be no use of this flag in this os
    GDT_FLAG_32BIT       0b0100,      // use 32-bit segment
    GDT_FLAG_GRANULARITY 0b1000       // scale the value of [limit] in the descriptor by 4kb, if not set [limit] is scaled by 1
} gdt_flags_t;

typedef struct 
{
    u32_t limit_16: 16;
    u32_t base_24:  24;
    u32_t access:   8;
    u32_t limit_4:  4;
    u32_t flags:    4;
    u32_t base_8:   8;
} __attribute__ ((packed)) gdt_entry_t;

#endif