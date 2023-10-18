// Programable Interval Timer // ~ eylon

#include <cpu/timer.h>
#include <cpu/interrupts/isr.h>
#include <drivers/ports.h>
#include <utils/type.h>

u32_t tick = 0;

static void timer_callback() {
    tick++;
}

// Convert Hertz to timer devider
static inline u16_t hz_to_devider(u32_t hz)
{
    return (CLOCK_HZ / hz);
}

// Set timer rate by Hertz
void set_timer_hz(u32_t hz)
{
    u16_t devider = hz_to_devider(hz);              // get the devider
    port_outb(PIT_CH0_DATA_PORT, LOW_8(devider));   // write the low byte of the devider
    port_outb(PIT_CH0_DATA_PORT, HIGH_8(devider));  // write the high byte of the devider
}

// Initiate the timer
void init_timer(u32_t hz)
{
    port_outb(PIT_CMD_PORT, PIT_MOD_BIN | PIT_MOD_OP3 | PIT_MOD_ACS_LH | PIT_MOD_CH0);
    set_timer_hz(hz);

    set_interrupt_handler(IRQ0, timer_callback);
}