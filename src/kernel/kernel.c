// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <drivers/ps2.h>
#include <cpu/interrupts/isr.h>
#include <cpu/timer.h>

void kernel_main() {
	kprint("Hello from kernel", VGA_TXT_RED | VGA_BG_GREEN);
	init_interrupt();
	//init_ps2();
	//init_timer(20);

	//__asm__ __volatile__ ("int $32");

	// Halt forever :: wait for an interrupt, execute it and continue halting
	for (;;) {
		__asm__ __volatile__ ("hlt");
	}

	// Should never execute this
	kprint("End of kernel", VGA_TXT_RED | VGA_BG_BLUE);
}