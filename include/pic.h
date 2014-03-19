#ifndef _PIC_H
#define _PIC_H

#include <defs.h>
//void initialize_pic(uint8_t, uint8_t);
void init_pic(uint8_t, uint8_t);
// Hardware Interrupt  Number
#define PIC_IRQ_TIMER 0
#define PIC_IRQ_KEYBOARD 1

// Operation Command Word 2
// For Deciding the Levels and for EOI
#define PIC_OCW2_LEVEL1        1
#define PIC_OCW2_LEVEL2        2
#define PIC_OCW2_LEVEL3        4
#define PIC_OCW2_EOI           0x20
#define PIC_OCW2_SELECTION     0x40
#define PIC_OCW2_ROTATION      0x80

// Operation Command Word 3
// Read ISR, IRR, set RESET Special Mask
#define PIC_OCW3_READ_ISR      1
#define PIC_OCW3_READ_ISS      2
#define PIC_OCW3_POLL          4
#define PIC_OCW3_SMM           0x20
#define PIC_OCW3_ESMM          0x40
#define PIC_OCW3_HIGHEST_BIT   0x80

// Initialization Control Word 1

#define PIC_ICW1_ICW4        1
#define PIC_ICW1_SINGLE_CASCADED        2
#define PIC_ICW1_ADI        4
#define PIC_ICW1_LEVEL_EDGE_TRIGGERED           0x20
#define PIC_ICW1_INIT     0x40
#define PIC_ICW1_HIGHEST_BIT      0x80

// Initialization Control Word 2

#define PIC_ICW2_LEVEL1        1
#define PIC_ICW2_LEVEL2        2
#define PIC_ICW2_LEVEL3        4
#define PIC_ICW2_EOI           0x20
#define PIC_ICW2_SELECTION     0x40
#define PIC_ICW2_ROTATION      0x80

// Initialization Control Word 3
// Setting pin 2 to pin 4

#define PIC1_ICW3     0x40
#define PIC2_ICW3     0x20

// Commands Word 4
// Only Setting x86 Mode
#define PIC1_ICW4     1
#define PIC2_ICW4     1

#define PIC1_REG_COMMAND      0x20
#define PIC1_REG_STATUS       0x20
#define PIC1_REG_DATA         0x21
#define PIC1_REG_IMR          0x21

#define PIC2_REG_COMMAND      0xA0
#define PIC2_REG_STATUS       0xA0
#define PIC2_REG_DATA         0xA1
#define PIC2_REG_IMR          0xA1

#endif
