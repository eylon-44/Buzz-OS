// PATA Driver // ~ eylon

#include <drivers/pata.h>
#include <libc/stdint.h> 
#include <drivers/ports.h>

/* Wait for the disk to be ready.
    Read the status register; we only care about bits 6 and 7
        bit 7 = 0 = Not busy
        bit 6 = 1 = Drive is ready */
static void wait_disk() {
    while ((port_inb(PATA_STATUS_PORT) & 0b11000000) != 0b01000000) {}
}

// Read a sector [lba] from the disk into [dest]
static void read_sector(void* dest, sector_t lba)
{
    wait_disk();                                                        // wait for disk to be ready

    port_outb(PATA_SECTOR_COUNT_PORT, 1);                               // set sector count to 1
    port_outb(PATA_LBA1_PORT,  (uint8_t) lba & 0xFF);                   // set bits 0-7 of lba
    port_outb(PATA_LBA2_PORT,  (uint8_t) (lba >> 8) & 0xFF);            // set bits 8-15 of lba
    port_outb(PATA_LBA3_PORT,  (uint8_t) (lba >> 16) & 0xFF);           // set bits 16-23 of lba
    port_outb(PATA_DRIVE_PORT, (uint8_t) ((lba >> 24) | 0xE0) & 0xFF);  // set bits 24-27 of lba and drive number
    port_outb(PATA_CMD_PORT, PATA_READ_CMD);                            // send read command

    wait_disk();                                                        // wait for disk to be ready
    insd(PATA_DATA_PORT, dest, PATA_SECTOR_SIZE / 4);                   // copy the sector from the PATA data port into RAM
}

/* Read [size] sectors from offset [disk_offset] in the disk into [dest] in memory.
    [disk_offset] is the sector offset in the disk; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    [size] is the amount of sectors to load; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    [dest] should be 4 byte aligned */
void pata_read_disk(void* dest, sector_t size, sector_t disk_offset)
{
    uint8_t* start = (uint8_t*) dest; 
    uint8_t* end   = start + size * PATA_SECTOR_SIZE;

    for (; start < end; start += PATA_SECTOR_SIZE, disk_offset++) {
        read_sector(start, disk_offset);
    }
}

// Read a sector [lba] from the disk into [src]
static void write_sector(void* src, sector_t lba)
{
    wait_disk();                                                        // wait for disk to be ready

    port_outb(PATA_SECTOR_COUNT_PORT, 1);                               // set sector count to 1
    port_outb(PATA_LBA1_PORT,  (uint8_t) lba & 0xFF);                   // set bits 0-7 of lba
    port_outb(PATA_LBA2_PORT,  (uint8_t) (lba >> 8) & 0xFF);            // set bits 8-15 of lba
    port_outb(PATA_LBA3_PORT,  (uint8_t) (lba >> 16) & 0xFF);           // set bits 16-23 of lba
    port_outb(PATA_DRIVE_PORT, (uint8_t) ((lba >> 24) | 0xE0) & 0xFF);  // set bits 24-27 of lba and drive number
    port_outb(PATA_CMD_PORT, PATA_WRITE_CMD);                           // send write command

    wait_disk();                                                        // wait for disk to be ready
    // Write into the PATA data port from RAM
    for (size_t i = 0; i < PATA_SECTOR_SIZE / 4; i++) {
        outsd(PATA_DATA_PORT, (uint32_t*) src + i, 1);
        io_wait();
    }

    port_outb(PATA_CMD_PORT, PATA_FLUSH_CMD);                           // flush the cache
    wait_disk();                                                        // wait for disk to be ready
}

/* Write [size] sectors from [src] into offset [disk_offset] in the disk.
    [disk_offset] is the sector offset in the disk; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    [size] is the amount of sectors to load; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    [src] should be 4 byte aligned */
void pata_write_disk(void* src, sector_t size, sector_t disk_offset)
{
    uint8_t* start = (uint8_t*) src; 
    uint8_t* end   = start + size * PATA_SECTOR_SIZE;

    for (; start < end; start += PATA_SECTOR_SIZE, disk_offset++) {
        write_sector(start, disk_offset);
    }
}