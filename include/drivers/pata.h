// PATA Driver Header File // ~ eylon

#if !defined(PATA_H)
#define PATA_H

#include <libc/stdint.h> 

/* Constants */

// Sector size in bytes
#define PATA_SECTOR_SIZE            0x200


/* PATA Ports */

// [W/R] Data port
#define PATA_DATA_PORT              0x1F0

// [W/R] Sector count port
#define PATA_SECTOR_COUNT_PORT      0x1F2

// [W/R] LBA1 port
#define PATA_LBA1_PORT              0x1F3
// [W/R] LBA2 port
#define PATA_LBA2_PORT              0x1F4
// [W/R] LBA3 port
#define PATA_LBA3_PORT              0x1F5
// [W/R] Drive port
#define PATA_DRIVE_PORT             0x1F6

// [R] Status port
#define PATA_STATUS_PORT            0x1F7
// [W] Command port
#define PATA_CMD_PORT               0x1F7

#define PATA_READ_SECTOR            0x20

void pata_read_disk(void* dest, uint32_t size, uint32_t disk_offset);

#endif