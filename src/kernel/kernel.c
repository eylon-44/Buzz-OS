// Buzz OS Kernel Main // ~ eylon

#include <drivers/screen.h>
#include <cpu/interrupts/isr.h>

void kernel_main() {
	kprint("Hello from kernel", VGA_TXT_RED | VGA_BG_GREEN);
	init_interrupt();

	__asm__ __volatile__ ("hlt");

	kprint("End of kernel", VGA_TXT_RED | VGA_BG_BLUE);
}