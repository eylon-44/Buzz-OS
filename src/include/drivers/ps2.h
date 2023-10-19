// PS/2 Controller Driver Header File // ~ eylon

#if !defined(PS2_H)
#define PS2_H

// IO ports
#define PS2_DATA_PORT   0x60                // data port (read/write)
#define PS2_STATUS_PORT 0x64                // status register (read)
#define PS2_CMD_PORT    0x64                // command port (write)

// PS/2 controller commands
#define PS2_TEST_PS2   0xAA                 // test ps/2 controller | response 0x55:passed 0xFC:failed
#define PS2_TEST_PORT1 0xAB                 // test ps/2 port one   | response 0x00:passed 0x01-0x04:failed
#define PS2_TEST_PORT2 0xA9                 // test ps/2 port two   | response 0x00:passed 0x01-0x04:failed

#define PS2_ENABLE_PORT1  0xAE              // enable the first ps/2 port
#define PS2_DISABLE_PORT1 0xAD              // disable the first ps/2 port

void init_ps2();

#endif