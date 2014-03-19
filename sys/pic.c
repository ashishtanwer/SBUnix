#include <pic.h>
#include <defs.h>
#include <inline.h>
#define  ZEROING 0x0
#include <pti.h>
#include <stdio.h>
void init_pic(uint8_t, uint8_t);
void pic_send_command(uint8_t, uint8_t);
void pic_send_data(uint8_t, uint8_t);
uint8_t pic_read_data(uint8_t);
/*
 Code Referenced by : Broken Thorn http://www.brokenthorn.com/
 */
void pic_send_command(uint8_t cmd, uint8_t picNum)
{
    if (picNum > 1)
        return;
    uint8_t register_command;
    if (picNum == 1)
    {
        register_command = PIC2_REG_COMMAND;
    }
    else
    {
        register_command = PIC1_REG_COMMAND;
    }
    outportb(register_command, cmd);
}
//! send data to PICs
void pic_send_data(uint8_t data, uint8_t picNum)
{
    if (picNum > 1)
        return;
    uint8_t data_command;
    if (picNum == 1)
    {
        data_command = PIC2_REG_DATA;
    }
    else
    {
        data_command = PIC1_REG_DATA;
    }
    outportb(data_command, data);
}
//! read data from PICs
uint8_t pic_read_data(uint8_t picNum)
{
    if (picNum > 1)
        return 0;
    uint8_t reg;
    if (picNum == 1)
    {
        reg = PIC2_REG_DATA;
    }
    else
    {
        reg = PIC1_REG_DATA;
    }
    return inportb(reg);
}
void disable()
{
    printk("\ndisabling ...");
    pic_send_data(0xff, 0);
    pic_send_data(0xff, 1);
}
/*
 Code Referenced by : Broken Thorn http://www.brokenthorn.com/
 */
//! Initialize pic
void init_pic(uint8_t base0, uint8_t base1)
{
    pic_send_command(PIC_ICW1_ICW4 | PIC_ICW1_INIT, 0);
    pic_send_command(PIC_ICW1_ICW4 | PIC_ICW1_INIT, 1);
    pic_send_data(base0, 0);
    pic_send_data(base1, 1);
    pic_send_data(PIC1_ICW3, 0);
    pic_send_data(PIC2_ICW3, 1);
    pic_send_data(PIC1_ICW4, 0);
    pic_send_data(PIC2_ICW4, 1);
    outportb(0x21, ZEROING);
    outportb(0xA1, ZEROING);
    __asm__ __volatile__("sti");
}
void init_timer()
{
    int hz = 100;
    uint32_t divisor = 1193180 / hz;
    // Send the command byte.
    outportb(0x43, 0x36);
    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t) (divisor & 0xFF);
    uint8_t h = (uint8_t) ((divisor >> 8) & 0xFF);
    // Send the frequency divisor.
    outportb(0x40, l);
    outportb(0x40, h);
}
