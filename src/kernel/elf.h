// ELF Header File // ~ eylon

#if !defined(ELF_H)
#define ELF_H

#include <utils/type.h>

#define ELF_MAGIC 0x7F454C46

// ELF file header
struct ElfHeader {
    uint32_t magic;           // [byte 0-3]   the magic number identifies the file as an ELF object file
    uint8_t  bits;            // [byte 4]     <1>=32-bit <2>=64-bit
    uint8_t  endian;          // [byte 5]     <1>=little-endian <2>=big-endian
    uint8_t  header_version;  // [byte 6]     ELF header version
    uint8_t  _os_abi;         // [byte 7]     OS ABI -> 0
    uint8_t  _unused[8];      // [byte 8-15]  unused -> 0
    uint16_t type;            // [byte 16-17] ELF type <1>=relocatable <2>=executable <3>=shared <4>=core 
    uint16_t architecture;    // [byte 18-19] instruction set architecture <3>=x86
    uint32_t version;         // [byte 20-23] ELF version
    uint32_t entry;           // [byte 24-27] program's entry address
    uint32_t pht_offset;      // [byte 28-31] program header table offset
    uint32_t sht_offset;      // [byte 32-35] section header table position
    uint32_t _flags;          // [nb  byte 36-39] flags are ignored for x86 ELFs
    uint16_t header_size;     // [byte 40-41] size of the ELF header in bytes
    uint16_t pht_entry_size;  // [byte 42-43] size of an entry in the program header table
    uint16_t pht_entry_count; // [byte 44-45] number of entries in the program header table
    uint16_t sht_entry_size;  // [byte 46-47] size of an entry in the section header table
    uint16_t sht_entry_count; // [byte 48-49] number of entries in the section header table
    uint16_t sht_names_index; // [byte 50-51] index in section header table with the section names
};

// ELF program header (segment)
struct ElfPrgHeader {
    uint32_t type;            // [byte 0-3]   segment type
    uint32_t offset;          // [byte 4-7]   segment data offset in the file
    uint32_t vaddr;           // [byte 8-11]  virutal address  (VMA)
    uint32_t paddr;           // [byte 12-15] physical address (LMA)
    uint32_t file_size;       // [byte 16-19] size of the segment in the file
    uint32_t mem_size;        // [byte 20-23] size of the segment in memory
    uint32_t flags;           // [byte 24-27] segment flags
    uint32_t alignment;       // [byte 28-31] required alignment for this segment (must be a power of 2)
};

// ELF section header
struct ElfSecHeader{
    uint32_t name;            // [byte 0-3]   offset to section name in string table
    uint32_t type;            // [byte 4-7]   section type
    uint32_t flags;           // [byte 8-11]  section flags
    uint32_t addr;            // [byte 12-15] virtual address in memory
    uint32_t offset;          // [byte 16-19] byte offset from the beginning of the file to the first byte in the section
    uint32_t size;            // [byte 20-23] size of the section
    uint32_t link;            // [byte 24-27] link index to another section
    uint32_t info;            // [byte 28-31] additional information
    uint32_t alignment;       // [byte 32-35] slignment of section
    uint32_t entrt_size;      // [byte 36-39] size for entries in a section that is a table of fixed-size entries
};

#endif
