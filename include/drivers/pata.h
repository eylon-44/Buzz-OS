// PATA Driver Header File // ~ eylon

#if !defined(PATA_H)
#define PATA_H

#include <libc/stdint.h> 
#include <stddef.h>

// Sector size in bytes
#define PATA_SECTOR_SIZE            0x200

// [W/R] Data port
#define PATA_DATA_PORT              0x1F0

// [W/R] Sector count port
#define PATA_SECTOR_COUNT_PORT      (PATA_DATA_PORT + 2)

// [W/R] LBA1 port
#define PATA_LBA1_PORT              (PATA_DATA_PORT + 3)
// [W/R] LBA2 port
#define PATA_LBA2_PORT              (PATA_DATA_PORT + 4)
// [W/R] LBA3 port
#define PATA_LBA3_PORT              (PATA_DATA_PORT + 5)
// [W/R] Drive port
#define PATA_DRIVE_PORT             (PATA_DATA_PORT + 6)

// [R] Status port
#define PATA_STATUS_PORT            (PATA_DATA_PORT + 7)
// [W] Command port
#define PATA_CMD_PORT               (PATA_DATA_PORT + 7)

#define PATA_READ_CMD               0x20
#define PATA_WRITE_CMD              0x30
#define PATA_FLUSH_CMD              0xE7

// 1 sector = 512 bytes
typedef size_t sector_t;

void pata_read_disk(void* dest, sector_t size, sector_t disk_offset);
void pata_write_disk(void* src, sector_t size, sector_t disk_offset);

#endif