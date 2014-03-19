#include<defs.h>
#include<stdio.h>
#include<keyboard.h>
#include<inline.h>
#include<idt.h>
#include<handler.h>
#include<stdio.h>
#include<stdarg.h>
#include<process.h>
#include<vmm.h>
#include<scheduler.h>
#include<ahci.h>
#include<e1000.h>
char buf[100];
int buf_length = 0;
#define SCANCODE_SHIFT 42
unsigned char get_other_characters(unsigned char value)
{
    if (value == '/')
    {
        value = '?';
    }
    else if (value == '[')
    {
        value = '{';
    }
    else if (value == ';')
    {
        value = ':';
    }
    else if (value == '.')
    {
        value = '>';
    }
    else if (value == ',')
    {
        value = '<';
    }
    else if (value == '\'')
    {
        value = '"';
    }
    else if (value == ']')
    {
        value = '}';
    }
    else if (value == '\\')
    {
        value = '|';
    }
    else
    {
        value = value - 32;
    }
    return value;
}
void handler_keyboard_routine()
{
    static uint8_t isShiftPressed = 0;
    unsigned char scancode;
    scancode = inportb(0x60);
    if (scancode == 28)
    {
        int i = 0;
        for (; i < buf_length; i++)
        {
        }
        buf[buf_length] = '\0';
        find_terminal_process(buf);
        buf_length = 0;
    }
    else
    {
        if (scancode & 0x80)
        {
            if (scancode - 0x80 == SCANCODE_SHIFT)
                isShiftPressed = 0;
        }
        else
        {
            if (scancode == SCANCODE_SHIFT)
                isShiftPressed = 1;
            else
            {
                unsigned char value = kbdus[scancode];
                if (isShiftPressed == 1)
                {
                    if (scancode >= 2 && scancode <= 13)
                        value = shiftSpecial[scancode - 2];
                    else
                    {
                        value = get_other_characters(value);
                    }
                }
                buf[buf_length] = value;
                buf_length++;
                printk("%c", value);
            }
        }
    }
}
