// Programable Interrupt Controller Utils // ~ eylon

#include <cpu/interrupts/pic.h>
#include <drivers/ports.h>
#include <utils/type.h>

// Initialize and config the PIC
void init_pic()
{
    // ICW1 :: start the Interrupt Control Word initiatlization sequence
    port_outb(PIC1_CMD_PORT, ICW1_INIT | ICW1_ICW4);
    port_outb(PIC2_CMD_PORT, ICW1_INIT | ICW1_ICW4);

    // ICW2 :: set the vector offsets
    port_outb(PIC1_DATA_PORT, ICW2_PIC1);
    port_outb(PIC2_DATA_PORT, ICW2_PIC2);

    // ICW3 :: pics connenction data and cascade mode
    port_outb(PIC1_DATA_PORT, ICW3_PIC1);
    port_outb(PIC2_DATA_PORT, ICW3_PIC2);

    // ICW4 :: have the PICs use 8086 mode (and not 8080 mode)
    port_outb(PIC1_DATA_PORT, ICW4_8086);
    port_outb(PIC2_DATA_PORT, ICW4_8086);

    // Disable all IRQs except IRQ2 on master PIC
    mask_all_irq();
    unmask_irq(2);

    // [MAY] cause a problem on real hardware when not using an io_wait function
}

// [MAY] need to handle "Spurious IRQs" using the ISR (in service register), more likely to happen on real hardware

// Send the PIC an EOI (end of interrupt) signal :: take the interrupt number (0-15) as parameter
void pic_eoi(u8_t interrupt)
{
    // if interrupt number is out of range ignore request
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return; // [TODO] handle error
    }

    // if is a PIC2 interrupt
    if (interrupt >= PIC2_START_INTERRUPT) {
        port_outb(PIC2_CMD_PORT, PIC_EOI);      // send an EOI signal to slave
    }
    
    //port_outb(PIC1_CMD_PORT, PIC_EOI);          // send an EOI signal to master
    port_outb(0x20, 0x20);
}


// Mask (disable) all IRQs
void mask_all_irq()
{
    port_outb(PIC1_DATA_PORT, 0xFF); 
    port_outb(PIC2_DATA_PORT, 0xFF);
}

// Unmask (enable) all IRQs
void unmask_all_irq()
{
    port_outb(PIC1_DATA_PORT, 0x00); 
    port_outb(PIC2_DATA_PORT, 0x00); 
}

// Mask (disable) an IRQ by its index
void mask_irq(u8_t irq_line)
{
    u8_t value;
    u16_t port;

    // PIC1 irq
    if (irq_line < 8) {
        port = PIC1_DATA_PORT;
    }
    // PIC2 irq
    else {
        port = PIC2_DATA_PORT;
        irq_line -= 8;
    }

    value = port_inb(port) | (0b00000001 << irq_line);  // calculate the new IMR value
    port_outb(port, value);                             // set the IMR with the new value
    
}

// Unmask (enable) an IRQ by its index
void unmask_irq(u8_t irq_line)
{
    u8_t value;
    u16_t port;

    // PIC1 irq
    if (irq_line < 8) {
        port = PIC1_DATA_PORT;
    }
    // PIC2 irq
    else {
        port = PIC2_DATA_PORT;
        irq_line -= 8;
    }

    value = port_inb(port) & ~(0b00000001 << irq_line);  // calculate the new IMR value
    port_outb(port, value);                              // set the IMR with the new value
}