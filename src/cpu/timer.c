// Programable Interval Timer // ~ eylon

#include <cpu/timer.h>
#include <cpu/interrupts/isr.h>
#include <drivers/ports.h>
#include <libc/stdint.h>
#include <libc/stddef.h>

uint32_t tick = 0;

static void timer_callback(UNUSED int_data_t* _) {
    tick++;
}

// Convert Hertz to timer devider
static inline uint16_t hz_to_devider(uint32_t hz)
{
    return (CLOCK_HZ / hz);
}

// Set timer rate by Hertz
static void set_timer_hz(uint32_t hz)
{
    uint16_t devider = hz_to_devider(hz);           // get the devider
    port_outb(PIT_CH0_DATA_PORT, low16(devider));   // write the low byte of the devider
    port_outb(PIT_CH0_DATA_PORT, high16(devider));  // write the high byte of the devider
}

// Initiate the timer
void init_timer()
{
    port_outb(PIT_CMD_PORT, PIT_MOD_BIN | PIT_MOD_OP3 | PIT_MOD_ACS_LH | PIT_MOD_CH0);
    set_timer_hz(TICK_HZ);

    set_interrupt_handler(IRQ0, timer_callback);
}