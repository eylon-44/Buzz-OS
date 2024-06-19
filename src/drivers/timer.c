// Programable Interval Timer // ~ eylon

#include <drivers/timer.h>
#include <kernel/interrupts/isr.h>
#include <drivers/ports.h>
#include <libc/stdint.h>
#include <kernel/process/scheduler.h>

// Array of void functions() to be called uppon a timer interrupt
static void (*callbacks[])() = { sched_tick };

/* This function is being called uppon a timer interrupt;
    it calls all of the functions in the [callbacks] array. */
static void timer_callback(UNUSED int_frame_t*) {
    for (size_t i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++) {
        callbacks[i]();
    }
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
    set_timer_hz(TIMER_TICK_HZ);

    set_interrupt_handler(IRQ0, timer_callback);
}