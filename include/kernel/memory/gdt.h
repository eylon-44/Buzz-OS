// Global Descriptor Table Header File // ~ eylon

#if !defined(GDT_H)
#define GDT_H

#include <libc/stdint.h>

// Segment selectors
#define GDT_KCODE_SEG   0x08 | 0x00
#define GDT_KDATA_SEG   0x10 | 0x00
#define GDT_UCODE_SEG   0x18 | 0x03
#define GDT_UDATA_SEG   0x20 | 0x03
#define GDT_TSS_SEG     0x28 | 0x00
#define SEG_COUNT       5

// Macro for easy gdt entry setup
#define GDT_ENTRY(base, limit, dpl, access_options, flags_options) \
    ((gdt_entry_t) { \
        .base_24  = (base) & 0xFFFFFF, \
        .base_8   = (base) >> 24 & 0xFF, \
        .limit_16 = (limit) & 0xFFFF, \
        .limit_4  = (limit) >> 16 & 0xF, \
        .access   = (uint8_t) (0 | (access_options) | (dpl << 5 & 0b01100000)), \
        .flags    = (uint8_t) (flags_options) \
    })

// GDT entry structure
typedef struct 
{
    uint32_t limit_16: 16;
    uint32_t base_24:  24;
    uint32_t access:   8;
    uint32_t limit_4:  4;
    uint32_t flags:    4;
    uint32_t base_8:   8;
} __attribute__ ((packed)) gdt_entry_t;

// GDT descriptor structure :: gdt size [16 bits], gdt address [32 bits]
typedef struct
{
    uint16_t gdt_size;
    uint32_t gdt_address;
} __attribute__((packed)) gdt_descriptor_t;

// [gdt_entry_t] access attribute options
typedef enum
{
    GDT_ACCESS_ACCESSED       = 0b00000001,         // [bit 0] the accessed bit is being set by the cpu when the segment is accessed and the bit is not already set to 1

    GDT_ACCESS_READABLE_CODE  = 0b00000010,         // [bit 1] <code seg> readable bit   :: <0>=read access is forbidden <1>=read access is allowed
    GDT_ACCESS_WRITEABLE_DATA = 0b00000010,         // [bit 1] <data seg> writeable bit  :: <0>=write access is forbidden <1>=write access is allowed

    GDT_ACCESS_CONFORMING     = 0b00000100,         // [bit 2] <code seg> conforming bit :: code in this segment can be executed from an equal or lower privilege level
    GDT_ACCESS_DIRECTION_UP   = 0b00000000,         // [bit 2] <data seg> direction bit  :: <0>=grow up
    GDT_ACCESS_DIRECTION_DOWN = 0b00000100,         // [bit 2] <data seg> direction bit  :: <1>=grow down

    GDT_ACCESS_DATA_SEG       = 0b00000000,         // [bit 3] <0>=data segment
    GDT_ACCESS_CODE_SEG       = 0b00001000,         // [bit 3] <1>=code segment

    GDT_ACCESS_SYS_SEG        = 0b00000000,         // [bit 4] descriptor type :: <0>=system segment (TSS)
    GDT_ACCESS_NON_SYS_SEG    = 0b00010000,         // [bit 4] descriptor type :: <1>=non system segment -> code/data segment

    GDT_ACCESS_DPL_MASK       = 0b01100000,         // [bit 5-6] descriptor privilege level mask

    GDT_ACCESS_PRESENT        = 0b10000000          // [bit 7] present bit     :: <0>=not present -> invalid segment <1>=present -> valid segment
} gdt_access_flags_t;

// [gdt_entry_t] flags attribute options
typedef enum 
{
    GDT_FLAG_LONG        = 0b0010,      // use a long mode code segment, there will be no use of this flag in this os
    GDT_FLAG_32BIT       = 0b0100,      // use 32-bit segment
    GDT_FLAG_GRANULARITY = 0b1000       // scale the value of [limit] in the descriptor by 4kb, if not set, [limit] is scaled by 1
} gdt_flags_t;

void init_gdt();

#endif