// PATA Driver // ~ eylon

#include <drivers/pata.h>
#include <libc/stdint.h> 
#include <drivers/ports.h>

// Wait for the disk to be ready
static void wait_disk() {
    /* Read the status register; we only care about bits 6 and 7
        bit 7 = 0 = Not busy
        bit 6 = 1 = Drive is ready
    */
    while ((port_inb(PATA_STATUS_PORT) & 0b11000000) != 0b01000000) {}
}

// Read a sector at [lba] from the disk into [dest]
static void read_sector(void* dest, uint32_t lba)
{
    wait_disk();                                                    // wait for disk to be ready

    port_outb(PATA_SECTOR_COUNT_PORT, 1);                           // set sector count to 1
    port_outb(PATA_LBA1_PORT,  (uint8_t) lba & 0xFF);                  // set bits 0-7 of lba
    port_outb(PATA_LBA2_PORT,  (uint8_t) (lba >> 8) & 0xFF);           // set bits 8-15 of lba
    port_outb(PATA_LBA3_PORT,  (uint8_t) (lba >> 16) & 0xFF);          // set bits 16-23 of lba
    port_outb(PATA_DRIVE_PORT, (uint8_t) ((lba >> 24) | 0xE0) & 0xFF); // set bits 24-27 of lba and drive number
    port_outb(PATA_CMD_PORT, PATA_READ_SECTOR);                     // send read command

    wait_disk();                                                    // wait for disk to be ready
    insd(PATA_DATA_PORT, dest, PATA_SECTOR_SIZE / 4);               // copy the sector from the PATA data port into RAM
}

/* Read [size] sections from location [disk_offset] in disk into [dest].
    !!! [size] and [disk_offset] are in sector units (1 sector = PATA_SECTOR_SIZE = 512) !!!
    - [disk_offset] is the sector offset in the disk; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    - [size] is the amount of sectors to load; 1 sector is [PATA_SECTOR_SIZE] (512) bytes
    - [dest] should be 4 bytes aligned */
void pata_read_disk(void* dest, uint32_t size, uint32_t disk_offset)
{
    uint8_t* start = (uint8_t*) dest; 
    uint8_t* end   = start + size;
    uint32_t lba = disk_offset;

    for (; start < end; start += PATA_SECTOR_SIZE, lba++) {
        read_sector(start, lba);
    }
}