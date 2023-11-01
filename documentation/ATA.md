# PCI IDE/ATA DMA using LBA

If the title of this of this page `panic()`{.c} you, well, idk.

There are many modes and protocols that can be used to write an hard disk driver. I will implement a **PCI IDE/ATA DMA using LBA** driver. Fancy name huh?

### What is PCI?
[info]
### What is IDE/ATA?
[info]
### What is DMA?
[info]

Advanced Technology Attachment (ATA) is a standard physical interface for connecting storage devices within a computer.

#### Parallel/Serial ATA/ATAPI
IDE can connect up to 4 drives. Each drive can be one of the following:

- ATA (Serial): Used for most modern hard drives.
- ATA (Parallel): Commonly used for hard drives.
- ATAPI (Serial): Used for most modern optical drives.
- ATAPI (Parallel): Commonly used for optical drives.

Accessing an ATA/PATA drive works the same way as accessing a SATA drive. This also implicitly states that accessing a PATAPI ODD is the same as accessing a SATAPI ODD. An IDE driver does not need to know whether a drive is parallel or serial, it only has to know whether it's using ATA or ATAPI.

**I want to write an PCI IDE/ATA DMA driver**, which is not too hard, not too slow.

> [!Note]
Recently a new standard for ATA data transmission has emerged. It was named SATA, and the previous well-known forms of ATA were retroactively renamed PATA, "to reduce confusion" - while certainly causing it.


[PCI wiki os](https://wiki.osdev.org/PCI)
[PCI wiki](https://en.wikipedia.org/wiki/Peripheral_Component_Interconnect)

[PCI IDE Controller](phttps://wiki.osdev.org/PCI_IDE_Controller)

[DMA](https://en.wikipedia.org/wiki/Direct_memory_access)

[ATA DMA](https://wiki.osdev.org/ATA/ATAPI_using_DMA)

[ATA PIO (slow and gross but some concepts apply to DMA)](https://wiki.osdev.org/ATA_PIO_Mode)

[ATA R/W](https://wiki.osdev.org/ATA_read/write_sectors#Read_in_CHS_mode)

[LBA](https://en.wikipedia.org/wiki/Logical_block_addressing)

[TODO] take a look at ACHI