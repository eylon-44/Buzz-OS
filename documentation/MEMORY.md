# Protoype to save some info

[Introducction](https://wiki.osdev.org/Memory_management)

[MMU](https://wiki.osdev.org/MMU)
[Segmentation](https://wiki.osdev.org/Segmentation)
https://wiki.osdev.org/CPU_Registers_x86#Segment_Registers

[Paging](https://wiki.osdev.org/Paging)
[Great Article](https://hasinisama.medium.com/operating-system-paging-a2fe7ad7f03f)
[MMU/PMMU(paged memory management unit](https://wiki.osdev.org/MMU)
[TLB](https://wiki.osdev.org/TLB)
[Page Tables](https://wiki.osdev.org/Page_Tables)
[Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
[Identity Paging](https://wiki.osdev.org/Identity_Paging)
[Memory allocation](https://wiki.osdev.org/Memory_Allocation)
[Heap](https://wiki.osdev.org/Heap)

Tutorials:
[Writing a memory manager (malloc & free)](https://wiki.osdev.org/Writing_a_memory_manager)
[Simple Heap Implementation](https://wiki.osdev.org/User:Pancakes/SimpleHeapImplementation)
[Brendan's Memory Management Guide](https://wiki.osdev.org/Brendan%27s_Memory_Management_Guide)
[Paging](https://forum.osdev.org/viewtopic.php?t=12307)
[Malloc theory](https://forum.osdev.org/viewtopic.php?t=12022)
[Setting up paging](https://wiki.osdev.org/Setting_Up_Paging)
[Writing a Page Fram Allocator](https://wiki.osdev.org/Writing_A_Page_Frame_Allocator)

- **Page Directory (PD):** A table in memory which the MMU uses to find the page tables. Each index in the Page Directory is a pointer to a Page Table. It can contain references to 1024 Page Tables (PT).

- **Page Table (PT):** A table in memory that describes how the MMU should translate a certain range of addresses. Each index in a Page Table contains the physical memory address to which a certain page should be mapped. It can point to 1024 sections of physical memory called page frames (PF).

- **Page Frame (PF):** A fixed-length contiguous block of physical memory into which memory pages are mapped. Each page frame is 4096 byte large (but can also be more when PSE is enabled, see below).


> [!Important]
A **Page Directory** is an array of 1024 **Page Directory Entries**.
Each **Page Directory Entrie** points to a single **Page Table**.
A **Page Table** is an array of 1024 **Page Table Entries**.
Each **Page Table Entrie** points to a single **Page Frame**.
A **Page Frame** is a 4KB/MB block of data.

```
└── PD
    ├── PDE1 -> PT1
    │           ├── PTE1    -> PF
    │           ├── PTE2    -> PF
    │           ├── PTE...  -> PF
    │           └── PTE1024 -> PF
    ├── PDE2 -> PT2
    │           ├── PTE1    -> PF
    │           ├── PTE2    -> PF
    │           ├── PTE...  -> PF
    │           └── PTE1024 -> PF
    ├── PDE... -> PT...
    │           ├── PTE1    -> PF
    │           ├── PTE2    -> PF
    │           ├── PTE...  -> PF
    │           └── PTE1024 -> PF
    └── PDE1024 -> PT1024
                ├── PTE1    -> PF
                ├── PTE2    -> PF
                ├── PTE...  -> PF
                └── PTE1024 -> PF
```

**Page Directory**
```c
PDE page_directory[1024];
```

**Page Directory Entry**
```c
typedef struct {
  uint8_t present:1;        // [bit 0] present in RAM
  uint8_t rw:1;             // [bit 1] read/write
  uint8_t us:1;             // [bit 2] user/supervisor
  uint8_t pwt:1;            // [bit 3] page-level write-through
  uint8_t pcd:1;            // [bit 4] page-level cache-disabled
  uint8_t accessed:1;       // [bit 5] accessed
  uint8_t _avl0:1;          // [bit 6] available: ignored by cpu
  uint8_t ps:1;             // [bit 7] page size, 0=4KB 1=4MB, must be 0 for this struct
  uint8_t _avl1:4;          // [bit 8-11]  available: ignored by cpu
  uint32_t pt_address:20;   // [bit 12-31] physical address of 4KB aligned page table referenced by this entry
} PDE __attribute__((packed));
```

**Page Table**
```c
PTE page_table[1024];
```

**Page Table Entry**
```c
typedef struct {
  uint8_t present:1;        // [bit 0] present in RAM
  uint8_t rw:1;             // [bit 1] read/write
  uint8_t us:1;             // [bit 2] user/supervisor
  uint8_t pwt:1;            // [bit 3] page-level write-through
  uint8_t pcd:1;            // [bit 4] page-level cache-disabled
  uint8_t accessed:1;       // [bit 5] accessed
  uint8_t dirty:1;          // [bit 6] dirty
  uint8_t pat:1;            // [bit 7] page attribute table, must be 0 unless PAT supported
  uint8_t global:1;         // [bit 8] global translation
  uint8_t _avl0:3;          // [bit 9-11]  available: ignored by cpu
  uint32_t pf_address:20;   // [bit 12-31] physical address of 4KB page frame
} PTE __attribute__((packed));
```


**Page Frame**
```c
// 4KB/MB block of data in physical memory
```

Paging in x86 consists of a page directory (PD) that can contain references to 1024 page tables (PT), each of which can point to 1024 sections of physical memory called page
frames (PF). Each page frame is 4096 byte large.

<img src=https://miro.medium.com/v2/resize:fit:1400/format:webp/1*c6gmOkfQNCE-ay1mTy9LSA.png>

Translation of a virtual address into a physical address first involves dividing the virtual address into three parts: 
- The most significant 10 bits (bits 22-31) specify the index of the **page directory entry/PT index**.
- The next 10 bits (bits 12-21) specify the index of the **page table entry/PF index**.
- The least significant 12 bits (bits 0-11) specify the **page offset/adress within the PF**.

The then MMU walks through the paging structures, starting with the page directory, and uses the page directory entry to locate the page table. The page table entry is used to locate the base address of the physical page frame, and the page offset is added to the physical base address to produce the physical address. If translation fails for some reason (entry is marked as not present, for example), then the processor issues a page fault :worried:.

<img src=https://miro.medium.com/v2/resize:fit:640/format:webp/1*6KY-IYu8Z0P7D1hjrr4u2Q.jpeg>

All page directories, page tables and page frames need to be aligned on 4096 byte addresses. This makes it
possible to address a PDT, PT or PF with just the highest 20 bits of a 32 bit address, since the lowest 12
need to be zero.

<img src="https://wiki.osdev.org/images/1/1e/Page_directory_entry.png">
<img src="https://wiki.osdev.org/images/6/60/Page_table_entry.png">

---
[Source](https://wiki.osdev.org/Paging#Page_Directory)
- **P**resent, If the bit is set, the page is actually in physical memory at the moment (in RAM). For example, when a page is swapped out (in Hard Disk), it is not in physical memory and therefore not 'Present'. If a page is called, but not present, a page fault will occur, and the OS should handle it. (See below.)

- **R**ead/**W**rite permissions flag. If the bit is set, the page is read/write. Otherwise when it is not set, the page is read-only. The [*W*rite*P*rotect](https://hadfiabdelmoumene.medium.com/change-value-of-wp-bit-in-cr0-when-cr0-is-panned-45a12c7e8411#:~:text=write%20protect%20(WP)%20is%20the,is%200%20(kernel%20mode)%20.) bit in CR0 determines if this is only applied to userland, always giving the kernel write access (the default) or both userland and the kernel (see Intel Manuals 3A 2-20).

- **U**ser/**S**upervisor bit, controls access to the page based on privilege level. If the bit is set, then the page may be accessed by all (rings 0-3); if the bit is not set, however, only the supervisor (ring 0) can access it. For a page directory entry, the user bit controls access to all the pages referenced by the page directory entry. Therefore if you wish to make a page a user page, you must set the user bit in the relevant page directory entry as well as the page table entry.
- **P**age-Level-**W**rite-**T**hrough abilities of the page. If the bit is set, [write-through caching](https://en.wikipedia.org/wiki/Cache_(computing)#Writing_policies) is enabled. If not, then write-back is enabled instead. The first option is prefered in most cases.

- **P**age-Level-**C**ache-**D**isable bit. If the bit is set, the page will not be cached. Otherwise, it will be. The second option (=0) is more common.

- **A**ccessed is used to discover whether a PDE or PTE was read during virtual address translation. If it has, then the bit is set, otherwise, it is not. Note that, this bit will not be cleared by the CPU, so that burden falls on the OS (if it needs this bit at all). Is set in both the accessed PTE and its PDE when accessing to a page frame in any way, read or write.

- **AV**ailab**L**e to software: bits for the OS to use for any purpose. These bits are not interpreted by the processor and are available for use by system software.

- **D**irty is used to determine whether a page has been written to.

- **P**age-**A**ttribute-**T**able. If PAT is supported, then PAT along with PCD and PWT shall indicate the memory caching type. Otherwise, it is reserved and must be set to 0. Won't be used in this project

- **G**lobal tells the processor not to invalidate the TLB entry corresponding to the page upon a MOV to CR3 instruction. Bit 7 (PGE) in CR4 must be set to enable global pages.

- **P**age-**S**ize stores the page size for that specific entry. If the bit is set, then the PDE maps to a page that is 4 MiB in size. Otherwise, it maps to a 4 KiB page table. Please note that 4-MiB pages require PSE to be enabled.

When **P**age-**S**ize=0, the page table address field represents the physical address of the page table that manages the four megabytes at that point. Please note that it is very important that this address be 4-KiB aligned. This is needed, due to the fact that the last 12 bits of the 32-bit value are overwritten by access bits and such. Similarly, when PS=1, the address must be 4-MiB aligned.

The remaining bits 9 through 11 (if PS=0, also bits 6 & 8) are not used by the processor, and are free for the OS to store some of its own accounting information. In addition, when P is not set, the processor ignores the rest of the entry and you can use all remaining 31 bits for extra information, like recording where the page has ended up in swap space. When changing the accessed or dirty bits from 1 to 0 while an entry is marked as present, it's recommended to invalidate the associated page. Otherwise, the processor may not set those bits upon subsequent read/writes due to TLB caching.

Setting the PS bit makes the page directory entry point directly to a 4-MiB page. There is no paging table involved in the address translation. Note: With 4-MiB pages, whether or not bits 20 through 13 are reserved depends on PSE being enabled and how many PSE bits are supported by the processor (PSE, PSE-36, PSE-40). CPUID should be used to determine this. Thus, the physical address must also be 4-MiB-aligned. Physical addresses above 4 GiB can only be mapped using 4 MiB PDEs.

---

## Enabling
Paging is enabled by first writing the address of the **Page Directory** to cr3 and then setting bit 31 (the PG
“paging-enable” bit) of cr0 to 1.

```nasm
mov eax, page_directory
mov cr3, eax        ; load the page directory address into cr3

mov eax, cr0        ; read current cr0
or eax, 0x80000000  ; set PG
mov cr0, eax        ; update cr0
; now paging is enabled
```
> [!Warning] 
setting the paging flag when the protection flag is clear causes a general protection exception. Also, once paging has been enabled, any attempt to enable long mode by setting LME (bit 8) of the EFER register will trigger a GPF. The CR0/PG must first be cleared before EFER/LME can be set.

To use 4 MB pages, set the PSE bit (Page Size Extensions, bit 4) of cr4.

```nasm
mov eax, cr4        ; read current cr4
or eax, 0x00000010  ; set PSE
mov cr4, eax        ; update cr4
```

## Error handling
[Page faults](https://wiki.osdev.org/Paging#Page_Faults)
A Page Fault is just an indication from the hardware that the required Virtual address is not mapped in the page table.

The page could simply have been swapped out. In that case the OS can service the page fault and the execution can continue as if nothing happened.

If the CPU find the page actually doens’t exist, it fires a page-not-present exception. The upper 10 bits specify the page directory entry (PDE) and the middle 10 bits specify the page table entry (PTE). First check the PDE and see if it’s present bit is set, if not setup a page table and point the PDE to the base address of the page table, set the present bit and iretd. If the PDE is present then the present bit of the PTE will be cleared. Then you’ll need to map some physical memory to the page table, set the present bit and then iretd to continue processing.

## Page Manipulation
[Self refrenced](https://wiki.osdev.org/Paging#Manipulation) is the ability to access the page directory
[Structures](https://wiki.osdev.org/Page_Tables)