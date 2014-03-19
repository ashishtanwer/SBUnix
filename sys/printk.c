#include<stdarg.h>
#include<defs.h>
#define MONITORLENGTH 25
#define MONITORWIDTH 80
int printk(char *, ...);
void starting_up_vga();
void add_character_to_screen(char);
void updateX_Y_for_New_Line();
void boundchecking();
void printd(va_list);
void prints(va_list);
void printp(va_list);
void printx(va_list);
void clrscreen();
void recalculate_offset();
//should be a constant
char *cursor_pos = (char *) 0xB8000;
char *start_pos = (char *) 0xB8000;
void clear_screen()
{
    char *start_tmp = start_pos;
    while (start_tmp <= ((char *) start_pos + 80 * 25 * 2))
    {
        *start_tmp = ' ';
        start_tmp = start_tmp + 2;
    }
    cursor_pos = start_pos;
}
//uint64_t VGA_buffer_address = 0xb8000;
char *starting_address_of_VGA_buffer = (char*) 0xb8000;
;
int position_x = 0;
int position_y = 0;
char *offset;
int attrib = 0x0F;
void starting_up_vga()
{
    uint16_t blank = 0x0f00;
    int i = 0;
    offset = starting_address_of_VGA_buffer;
    char* pointer = starting_address_of_VGA_buffer;
    for (i = 0; i < 2 * 24 * 80; i++)
    {
        *pointer = *(pointer + 160);
        pointer++;
    }
    for (i = 0; i < 160; i++)
    {
        *pointer = blank;
        pointer++;
    }
    position_x = 0;
    position_y = 24;
    recalculate_offset();
}
void recalculate_offset()
{
    offset = starting_address_of_VGA_buffer + 2 * position_y * 80
            + 2 * position_x;
}
int get_position_X()
{
    return position_x;
}
int get_position_Y()
{
    return position_y;
}
void set_position_X_Y(int positionX, int positionY)
{
    position_x = positionX;
    position_y = positionY;
    recalculate_offset();
}
void clrscreen()
{
    char* pointer = starting_address_of_VGA_buffer;
    int i = 0, j = 0;
    unsigned blank = 0x20 | (attrib << 8);
    for (i = 0; i < MONITORLENGTH; i++)
    {
        for (j = 0; j < MONITORWIDTH; j++)
        {
            *(pointer) = blank;
            pointer += 2;
        }
    }
}
int count = 0;
int print_user(char *format_specifier, va_list ap)
{
    count = 0;
    while (*format_specifier != '\0')
    {
        if (*format_specifier == '\n')
        {
            updateX_Y_for_New_Line();
        }
        if (*format_specifier == '%')
        {
            switch (*++format_specifier)
            {
                case 'd':
                    printd(ap);
                    break;
                case 'c':
                    add_character_to_screen(va_arg(ap, int));
                    break;
                case 's':
                    prints(ap);
                    break;
                case 'x':
                    //isHex = 1;
                    printx(ap);
                    break;
                case 'p':
                    printp(ap);
                    break;
            }
        }
        else
        {
            add_character_to_screen(*format_specifier);
        }
        format_specifier++;
    }
    return count;
}
int printk(char *format_specifier, ...)
{
    va_list ap;
    va_start(ap, format_specifier);
    count = 0;
    while (*format_specifier != '\0')
    {
        if (*format_specifier == '\n')
        {
            updateX_Y_for_New_Line();
        }
        if (*format_specifier == '%')
        {
            switch (*++format_specifier)
            {
                case 'd':
                    printd(ap);
                    break;
                case 'c':
                    add_character_to_screen(va_arg(ap, int));
                    break;
                case 's':
                    prints(ap);
                    break;
                case 'x':
                    //isHex = 1;
                    printx(ap);
                    break;
                case 'p':
                    printp(ap);
                    break;
            }
        }
        else
        {
            add_character_to_screen(*format_specifier);
        }
        format_specifier++;
    }
    va_end(ap);
    return count;
}
/*
 Adding a Character to the Screen
 */
void add_character_to_screen(char value)
{
    *offset = value;
    offset += 2; // increment the offset
    position_x += 1;
    count = count + 1;
    boundchecking();
}
/*
 Checking Bound Of the Screen
 */
void boundchecking()
{
    // finding if the end of line has reached
    //uint8_t attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & 0x0F);
    uint16_t blank = 0x0f00;
    // space  | (attributeByte << 8);
    int i = 0;
    // char c =' ';
    if (position_y >= MONITORLENGTH)
    {
        char* pointer = starting_address_of_VGA_buffer;
        for (i = 0; i < 2 * 24 * 80; i++)
        {
            *pointer = *(pointer + 160);
            pointer++;
        }
        for (i = 0; i < 160; i++)
        {
            *pointer = blank;
            pointer++;
        }
        position_x = 0;
        position_y = 24;
        recalculate_offset();
    }
    if (position_x == MONITORWIDTH)
    {
        updateX_Y_for_New_Line();
    }
}
/*
 Updating X and Y and OFFSET for NEW Line
 */
void updateX_Y_for_New_Line()
{
    position_x = 0;
    position_y += 1;
    //  offset = starting_address_of_VGA_buffer +  2*position_y*80;
    recalculate_offset();
}
void printd(va_list ap)
{
    int p = va_arg(ap, int);
    if (p == 0)
    {
        add_character_to_screen('0');
        return;
    }
    if (p < 0)
    {
        add_character_to_screen('-');
        p = -1 * p;
    }
    int tempP = p;
    int reverse_number = 0;
    while (p != 0)
    {
        reverse_number = reverse_number * 10 + p % 10;
        p = p / 10;
    }
    int temp = reverse_number;
    while (temp != 0)
    {
        //int temp2 = temp%10;
        //char a = temp
        char a = temp % 10 + '0';
        add_character_to_screen(a);
        temp = temp / 10;
    }
    while (tempP % 10 == 0)
    {
        char a = tempP % 10 + '0';
        add_character_to_screen(a);
        tempP = tempP / 10;
    }
}
void prints(va_list ap)
{
    char *p = va_arg(ap, char *);
    while (*p != '\0')
    {
        add_character_to_screen(*p);
        p++;
    }
}
void printx(va_list ap)
{
    long num = va_arg(ap, long);
    int rem[50], i = 0, length = 0;
    if (num < 0)
    {
        unsigned long number = num * -1;
        number = number ^ 0xFFFFFFFFFFFFFFFF;
        number = number + 1;
        while (number > 0)
        {
            rem[i] = number % 16;
            number = number / 16;
            i++;
            length++;
        }
    }
    else
    {
        while (num > 0)
        {
            rem[i] = num % 16;
            num = num / 16;
            i++;
            length++;
        }
    }
    add_character_to_screen((0 + '0'));
    add_character_to_screen(('x'));
    for (i = length - 1; i >= 0; i--)
    {
        switch (rem[i])
        {
            case 10:
                /*char a = 10 + '0';*/
                add_character_to_screen(('a'));
                break;
            case 11:
                /*char b = 11 + '0';*/
                add_character_to_screen(('b'));
                break;
            case 12:
                /* char c = 12 + '0';*/
                add_character_to_screen(('c'));
                break;
            case 13:
                // char d = 13 + '0';
                add_character_to_screen(('d'));
                break;
            case 14:
                //char e = 14 + '0';
                add_character_to_screen(('e'));
                break;
            case 15:
                //char f = 15 + '0';
                add_character_to_screen(('f'));
                break;
            default:
                //char rest = rem[i] + '0';
                add_character_to_screen((rem[i] + '0'));
        }
    }
}
void printp(va_list ap)
{
    unsigned int num = va_arg(ap, unsigned int);
    int rem[50], i = 0, length = 0;
    while (num > 0)
    {
        rem[i] = num % 16;
        num = num / 16;
        i++;
        length++;
    }
    add_character_to_screen((0 + '0'));
    add_character_to_screen(('x'));
    for (i = length - 1; i >= 0; i--)
    {
        switch (rem[i])
        {
            case 10:
                /*char a = 10 + '0';*/
                add_character_to_screen(('a'));
                break;
            case 11:
                /*char b = 11 + '0';*/
                add_character_to_screen(('b'));
                break;
            case 12:
                /* char c = 12 + '0';*/
                add_character_to_screen(('c'));
                break;
            case 13:
                // char d = 13 + '0';
                add_character_to_screen(('d'));
                break;
            case 14:
                //char e = 14 + '0';
                add_character_to_screen(('e'));
                break;
            case 15:
                //char f = 15 + '0';
                add_character_to_screen(('f'));
                break;
            default:
                //char rest = rem[i] + '0';
                add_character_to_screen((rem[i] + '0'));
        }
    }
}
