// PS/2 Controller Driver // ~ eylon

#include <drivers/ps2.h>
#include <drivers/ports.h>
#include <drivers/screen.h>

void init_ps2()
{
    // test ps/2 controller
    port_outb(PS2_CMD_PORT, PS2_TEST_PS2);
    if (port_inb(PS2_DATA_PORT) == 0x55) {
        
    }
}