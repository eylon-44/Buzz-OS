// PATA Driver Header File // ~ eylon

#if !defined(PATA_H)
#define PATA_H

/* Constants */

// Sector size in bytes
#define PATA_SECTOR_SIZE            0x200

/* PATA Ports */

// [W/R] Data port
#define PATA_DATA_PORT              0x1F0

// [W/R] Sector count port
#define PATA_SECTOR_COUNT_PORT      0x1F2

// [W/R] LBA-low port
#define PATA_LBA_LOW_PORT           0x1F3
// [W/R] LBA-mid port
#define PATA_LBA_MID_PORT           0x1F4
// [W/R] LBA-high port
#define PATA_LBA_HIGH_PORT          0x1F5
// [W/R] Drive port
#define PATA_DRIVE_PORT             0x1F6

// [R] Status port
#define PATA_STATUS_PORT            0x1F7
// [W] Command port
#define PATA_CMD_PORT               0x1F7

#endif