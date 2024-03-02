// Programable Interrupt Controller Header File // ~ eylon

#if !defined(PIC_H)
#define PIC_H

#include <utils/type.h>

// PICs ports
#define PIC1_CMD_PORT 0x20
#define PIC2_CMD_PORT 0xA0

#define PIC1_DATA_PORT 0x21
#define PIC2_DATA_PORT 0xA1

// Remapped PIC offset
#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   0x07 + PIC2_START_INTERRUPT

// End of interrupt
#define PIC_EOI 0x20

// Initialization Control Word
#define ICW1_ICW4	    0b00000001		// indicates that ICW4 will be present
#define ICW1_SINGLE	    0b00000010		// single (cascade) mode
#define ICW1_INTERVAL4	0b00000100		// call address interval 4 (8)
#define ICW1_LEVEL	    0b00001000	    // level triggered (edge) mode
#define ICW1_INIT	    0b00010000		// initialization - always required!
 
#define ICW2_PIC1  PIC1_START_INTERRUPT // vector offset for master
#define ICW2_PIC2  PIC2_START_INTERRUPT // vector offset for slave

#define ICW3_PIC1       0b00000100      // connection data to master
#define ICW3_PIC2       0b00000010      // connection data to slave

#define ICW4_8086		0b00000001	    // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO		0b00000010	    // auto (normal) EOI
#define ICW4_BUF_SLAVE	0b00001000	    // buffered mode/slave
#define ICW4_BUF_MASTER	0b00001100	    // buffered mode/master
#define ICW4_SFNM		0b00010000	    // special fully nested (not)

void init_pic();
void pic_eoi(u8_t interrupt);
void mask_irq(u8_t irq_line);
void unmask_irq(u8_t irq_line);
void mask_all_irq();
void unmask_all_irq();

#endif