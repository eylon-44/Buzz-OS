// PS/2 Controller Driver // ~ eylon

#include <drivers/ps2.h>
#include <drivers/ports.h>
#include <drivers/screen.h>

//#include <kernel/panic.h>

void init_ps2()
{
    // test ps/2 controller
    port_outb(PS2_CMD_PORT, PS2_TEST_PS2);
    if (port_inb(PS2_DATA_PORT) == 0xFC) {
        kprint("PS/2 controller test failed", VGA_TXT_GREEN);
    }

    // test ps/2 first device port
    // port_outb(PS2_CMD_PORT, PS2_TEST_PORT1);
    // if (port_inb(PS2_DATA_PORT != 0x00)) {
    //     kprint("PS/2 first device port test failed", VGA_TXT_GREEN);
    // }

    // enable first ps/2 device port
    port_outb(PS2_CMD_PORT, PS2_ENABLE_PORT1);

    port_outb(PS2_CMD_PORT, 0xD2);
    port_outb(PS2_DATA_PORT, 0xF4);

}