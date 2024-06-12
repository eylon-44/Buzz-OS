// Buzz OS Kernel Main // ~ eylon

#include <kernel/memory/mm.h>
#include <kernel/process/pm.h>
#include <kernel/panic.h>
#include <kernel/syscall.h>
#include <kernel/ui.h>
#include <drivers/screen.h>
#include <drivers/keyboard.h>
#include <kernel/interrupts/isr.h>
#include <kernel/fs.h>
#include <drivers/timer.h>

// Kernel main function :: kernel start
void kernel_main()
{
    init_mm();
    init_interrupt();
    init_syscall();
    init_fs();
    init_pm();
    init_keyboard();
    init_timer();
    init_ui();

    // Set the interrupt flag; enable interrupts; scheduler kicks in
    __asm__ volatile ("sti");

	// Halt forever; wait for an interrupt, handle it, continue halting
	for (;;) { __asm__ __volatile__ ("hlt"); }

	// This code should never run
	KPANIC("Kernel main: reached unreachable code.");
}