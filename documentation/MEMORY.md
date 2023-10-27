**Protoype to save some info**

[Introducction](https://wiki.osdev.org/Memory_management)

[MMU](https://wiki.osdev.org/MMU)

[Segmentation](https://wiki.osdev.org/Segmentation)
https://wiki.osdev.org/CPU_Registers_x86#Segment_Registers

[Paging](https://wiki.osdev.org/Paging)
[MMU/PMMU(paged memory management unit](https://wiki.osdev.org/MMU)
[TLB](https://wiki.osdev.org/TLB)
[Page Tables](https://wiki.osdev.org/Page_Tables)
[Page Frame Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
[Identity Paging](https://wiki.osdev.org/Identity_Paging)
[Memory allocation](https://wiki.osdev.org/Memory_Allocation)
[Heap](https://wiki.osdev.org/Heap)

Tutorials:
[Writing a memory manager](https://wiki.osdev.org/Writing_a_memory_manager)
[Brendan's Memory Management Guide](https://wiki.osdev.org/Brendan%27s_Memory_Management_Guide)
[Paging](https://forum.osdev.org/viewtopic.php?t=12307)
[Malloc theory](https://forum.osdev.org/viewtopic.php?t=12022)
[Setting up paging](https://wiki.osdev.org/Setting_Up_Paging)
[Writing a Page Fram Allocator](https://wiki.osdev.org/Writing_A_Page_Frame_Allocator)

Page Directory (PD), Page Table (PT), Page Frame (PF)

```
└── PD
    ├── PT1
    │   ├── PF1
    │   ├── PF2
    │   ├── PF...
    │   └── PF1024
    ├── PT2
    │   ├── PF1
    │   ├── PF2
    │   ├── PF...
    │   └── PF1024
    ├── PT...
    │   ├── PF1
    │   ├── PF2
    │   ├── PF...
    │   └── PF1024
    └── PT1024
        ├── PF1
        ├── PF2
        ├── PF...
        └── PF1024
```

Translation of a virtual address into a physical address first involves dividing the virtual address into three parts: 
- The most significant 10 bits (bits 22-31) specify the index of the page directory entry **(PT index)**.
- The next 10 bits (bits 12-21) specify the index of the page table entry **(PF index)**.
- The least significant 12 bits (bits 0-11) specify the page offset **(Adress within the PF)**.

The then MMU walks through the paging structures, starting with the page directory, and uses the page directory entry to locate the page table. The page table entry is used to locate the base address of the physical page frame, and the page offset is added to the physical base address to produce the physical address. If translation fails for some reason (entry is marked as not present, for example), then the processor issues a page fault :worried:.

<img src="https://wiki.osdev.org/images/1/1e/Page_directory_entry.png">
<img src="https://wiki.osdev.org/images/6/60/Page_table_entry.png">


## Enabling
Enabling paging is actually very simple. All that is needed is to load CR3 with the address of the page directory and to set the paging (PG) and protection (PE) bits of CR0.

```nasm
mov eax, page_directory
mov cr3, eax

mov eax, cr0
or eax, 0x80000001
mov cr0, eax
```
Note: setting the paging flag when the protection flag is clear causes a general protection exception. Also, once paging has been enabled, any attempt to enable long mode by setting LME (bit 8) of the EFER register will trigger a GPF. The CR0.PG must first be cleared before EFER.LME can be set.

If you want to set pages as read-only for both userspace and supervisor, replace 0x80000001 above with 0x80010001, which also sets the WP bit.

To enable PSE (4 MiB pages) the following code is required.

```nasm
mov eax, cr4
or eax, 0x00000010
mov cr4, eax
```

## Error handling
[Page faults](https://wiki.osdev.org/Paging#Page_Faults)