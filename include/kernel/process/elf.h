// ELF Header // ~ eylon

#if !defined(ELF_H)
#define ELF_H

#include <utils/type.h>

#define ELF_MAGIC 0x7F454C46

// ELF header
typedef struct __attribute__((packed))
{
    struct                  // [0-15] ELF identification
    {
        u32_t   magic;          // [0-3]    ELF magic
        u8_t    bitness;        // [4]      Bus width       <1>=32 <2>=64 
        u8_t    endian;         // [5]      Endian          <1>=little <2>=big
        u8_t    header_version; // [6]      ELF header version
        u8_t    abi;            // [7]      OS ABI          <0>=System-V
        u8_t    padding[8];     // [8-15]   Padding
    } identify;
    u16_t   file_type;      // [16-17]  Type            <1>=relocatable <2>=executable <3>=shared <4>=core
    u16_t   machine;        // [18-19]  Machine         <3>=x86
    u32_t   elf_version;    // [20-23]  ELF version
    u32_t   entry;          // [24-27]  Program entry address
    u32_t   phoff;          // [28-31]  Program header table offset in the file
    u32_t   shoff;          // [32-35]  Section header table offset in the file
    u32_t   flags;          // [36-39]  Flags; can be ignored for x86
    u16_t   header_size;    // [40-41]  This header size
    u16_t   phsize;         // [42-43]  Size of a single entry in the program header table
    u16_t   phnum;          // [44-45]  Number of entries in the program header table
    u16_t   shsize;         // [46-47]  Size of a single entry in the section header table
    u16_t   shnum;          // [48-49]  Number of entries in the section header table
    u16_t   e_shstrndx;     // [50-51]  Index in section header table with the section names

} elfheader_t;

// Program header
typedef struct __attribute__((packed))
{
    u32_t   seg_type;       // [0-3]    Segment type    <0>=null <1>=load <2>=dynamic <3>=interp <4>=note-section
    u32_t   offset;         // [4-7]    The offset in the file that the data for this segment can be found
    u32_t   vaddr;          // [8-11]   Base address for placing this segment in virtual memory
    u32_t   paddr;          // [12-15]  Base address for placing this segment in physical memory
    u32_t   filesz;         // [16-19]  Size of the segment in the file
    u32_t   memsz;          // [20-23]  Size of the segment in memory
    u32_t   flags;          // [24-27]  Flags           <1>=executable <2>=writable <4>=readable.
    u32_t   align;          // [28-31]  The required alignment for this segment; must be a power of 2
} prgheader_t;

/*

      <<< ELF File Format >>>
-----------------------------------
|           ELF Header            |
-----------------------------------
|      Program Header Table       |
-----------------------------------
|           Segment [1]           |
-----------------------------------
|           Segment [...]         |
-----------------------------------
|           Segment [N]           |
-----------------------------------
| Section Header Table [Optional] |
-----------------------------------

      <<< Segment Types >>>
> LOAD:     set [memsz] bytes at [vaddr] to 0, then copy [filesz] bytes from [offset] to [vaddr] 
> DYNAMIC:  requires dynamic linking
> INTERP:   contains a file path to an executable to use as an interpreter for the following segment

*/

#endif