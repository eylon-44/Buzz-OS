// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>
#include <kernel/panic.h>

// Kernel main function :: kernel start
void kernel_main() {
	// Print a welcome message
	clear_screen();
	kprint("Welcome to Buzz OS", VGA_ATR_DEFAULT);

	// Initiate all
	init_interrupt();
	init_keyboard();
	// init_timer(50);

	// Halt forever :: wait for an interrupt, execute it and continue halting
	for (;;) { __asm__ __volatile__ ("hlt"); }

	// Should never execute this
	panic("End of kernel");
}