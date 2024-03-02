// Buzz OS Kernel Main // ~ eylon

#include <kernel/memory/mm.h>
#include <kernel/panic.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>

// Kernel main function :: kernel start
void kernel_main() {
	// Print a welcome message
	clear_screen();
	kprint("Welcome to Buzz OS", VGA_ATR_DEFAULT);

	// Initiate all services
	init_interrupt();
	init_mm();

	init_keyboard();

	// Halt forever :: wait for an interrupt, handle it, continue halting
	for (;;) { __asm__ __volatile__ ("hlt"); }

	// Should never execute this
	panic("End of kernel");
}