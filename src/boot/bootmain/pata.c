// PATA Driver // ~ eylon

#include "pata.h"
#include <utils/type.h> 
#include <drivers/ports.h>

// Wait for the disk to be ready
static void wait_disk() {
    /* Read the status register; we only care about bits 6 and 7
        bit 7 = 0 = Not busy
        bit 6 = 1 = Drive is ready
    */
    while ((port_inb(PATA_STATUS_PORT) & 0b11000000) != 0b01000000) {}
}

void read_sector(void* dest, u32_t lba)
{
    wait_disk();                                        // wait for disk to be ready
    port_outb(PATA_SECTOR_COUNT_PORT, 1);               // set sector count as 1
    port_outb(PATA_LBA_LOW_PORT, (u8_t) lba);           // set lba-low
    port_outb(PATA_LBA_MID_PORT, (u8_t) (lba >> 8));    // set lba-mid
    port_outb(PATA_LBA_HIGH_PORT, (u8_t) (lba >> 16));  // set lba-high
    port_outb(PATA_CMD_PORT, 0x20);                     // send read command

    wait_disk();                                        // wait for disk to be ready
    insd(PATA_DATA_PORT, dest, PATA_SECTOR_SIZE / 4);   // read the data sector from the PATA data port into RAM
}

/* Read [size] bytes from location [disk_offset] in disk into [dest]
    - might read more data than requested 
    - [disk_offset] should be [PATA_SECTOR_SIZE] (512) bytes aligned */
void read_disk(u8_t* dest, u32_t size, u32_t disk_offset)
{
    // Determine the end address of written data
    u8_t* end = (dest + size);
    // Convert [disk_offset] bytes into sectors
    disk_offset /= PATA_SECTOR_SIZE;

    for (;dest < end; dest += PATA_SECTOR_SIZE, disk_offset++) {
        read_sector(dest, disk_offset);
    }
}