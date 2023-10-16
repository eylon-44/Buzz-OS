// Programable Interrupt Controller Utils // ~ eylon

#include <cpu/interrupts/pic.h>
#include <drivers/ports.h>
#include <utils/type.h>

// Initialize and config the PIC
void init_pic()
{
    u8_t mask_pic1, mask_pic2;

    // save the IMR (interrupt mask register) of both PICs
    mask_pic1 = port_inb(PIC1_MASK_PORT);
    mask_pic2 = port_inb(PIC2_MASK_PORT);

    // ICW1 :: start the Interrupt Control Word initiatlization sequence
    port_outb(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4);
    port_outb(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4);

    // ICW2 :: set the vector offsets
    port_outb(PIC1_CMD_PORT, ICW2_PIC1);
    port_outb(PIC2_CMD_PORT, ICW2_PIC2);

    // ICW3 :: pics connenction data and cascade mode
    port_outb(PIC1_CMD_PORT, ICW3_PIC1);
    port_outb(PIC2_CMD_PORT, ICW3_PIC2);

    // ICW4 :: have the PICs use 8086 mode (and not 8080 mode)
    port_outb(PIC1_CMD_PORT, ICW4_8086);
    port_outb(PIC2_CMD_PORT, ICW4_8086);

    // restore the IMR
    port_outb(PIC1_MASK_PORT, mask_pic1);
    port_outb(PIC2_MASK_PORT, mask_pic2);

    // [MAY] cause a problem on real hardware when not using an io_wait function
}

// [MAY] need to handle "Spurious IRQs" using the ISR (in service register), more likely to happen on real hardware

// Send the PIC an EOI (end of interrupt) signal :: take the interrupt number as parameter
void pic_eoi(u32_t interrupt)
{
    // if interrupt number is out of range
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return; // [TODO] add a panic command ? /[TODO] add the panic commnad under the kernel as panic.c
    }

    // if is a PIC2 interrupt
    if (interrupt >= PIC2_START_INTERRUPT) {
        port_outb(PIC2_CMD_PORT, PIC_EOI);      // send an end of interrupt signal  
    }
    
    port_outb(PIC1_CMD_PORT, PIC_EOI);          // send an end of interrupt signal
}


// Mask all IRQs
void mask_all_irq()
{
    port_outb(PIC1_MASK_PORT, 0xFF); 
    port_outb(PIC2_MASK_PORT, 0xFF);
}

// Unmask all IRQs
void unmask_all_irq()
{
    port_outb(PIC1_MASK_PORT, 0x00); 
    port_outb(PIC2_MASK_PORT, 0x00); 
}

// Mask an IRQ by its index
void mask_irq(u8_t irq_line)
{
    u8_t value;
    u16_t port;

    // PIC1 irq
    if (irq_line < 8) {
        port = PIC1_MASK_PORT;
    }
    // PIC2 irq
    else {
        port = PIC2_MASK_PORT;
        irq_line -= 8;
    }

    value = port_inb(port) | (0b00000001 << irq_line);  // calculate the new IMR value
    port_outb(port, value);                             // set the IMR with the new value
    
}

// Unmask an IRQ by its index
void unmask_irq(u8_t irq_line)
{
    u8_t value;
    u16_t port;

    // PIC1 irq
    if (irq_line < 8) {
        port = PIC1_MASK_PORT;
    }
    // PIC2 irq
    else {
        port = PIC2_MASK_PORT;
        irq_line -= 8;
    }

    value = port_inb(port) & ~(0b00000001 << irq_line);  // calculate the new IMR value
    port_outb(port, value);                              // set the IMR with the new value
}