// Programable Interval Timer Header File // ~ eylon

#if !defined(TIMER_H)
#define TIMER_H

#include <libc/stdint.h>

// 8 bit IO ports
#define PIT_CH0_DATA_PORT 0x40         // channel 0 data port (read/write)
#define PIT_CH1_DATA_PORT 0x41         // channel 1 data port (read/write)
#define PIT_CH2_DATA_PORT 0x42         // channel 2 data port (read/write)
#define PIT_CMD_PORT      0x43         // command port :: write to the mode register (write only)

// PIT command port options //

// (0)                            |
#define PIT_MOD_BIN      0b00000000     // binary mode
#define PIT_MOD_BCD      0b00000001     // bcd mode
// (1-3)                       |||
#define PIT_MOD_OP0      0b00000000     // interrupt on terminal count
#define PIT_MOD_OP1      0b00000010     // hardware re-triggerable one-shot
#define PIT_MOD_OP2      0b00000100     // rate generator
#define PIT_MOD_OP3      0b00000110     // sqaure wave generator
#define PIT_MOD_OP4      0b00001000     // software triggered strobe
#define PIT_MOD_OP5      0b00001010     // hardware triggered strobe
// (4-5)                     ||
#define PIT_MOD_ACS_LC   0b00000000     // latch count value command
#define PIT_MOD_ACS_LO   0b00010000     // low byte only
#define PIT_MOD_ACS_HO   0b00100000     // high byte only
#define PIT_MOD_ACS_LH   0b00110000     // low then high byte
// (6-7)                   || 
#define PIT_MOD_CH0      0b00000000     // select channel 0
#define PIT_MOD_CH1      0b01000000     // select channel 1
#define PIT_MOD_CH2      0b10000000     // select channel 2


// Hardware clock at 1193182 Hz with no devider
#define CLOCK_HZ          1193182       // Hz

void set_timer_hz(uint32_t hz);
void init_timer(uint32_t hz);

#endif