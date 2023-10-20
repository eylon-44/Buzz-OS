// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>
#include <kernel/panic.h>

// Kernel main function :: kernel start
void kernel_main() {
	kprint("Welcome to Buzz OS", VGA_TXT_RED | VGA_BG_GREEN);
	init_interrupt();

	// Halt forever :: wait for an interrupt, execute it and continue halting
	for (;;) { __asm__ __volatile__ ("hlt"); }

	// Should never execute this
	panic("End of kernel");
}