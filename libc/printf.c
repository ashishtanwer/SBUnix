#include<stdarg.h>
#include<defs.h>
void printd(va_list, char*, uint64_t*);
void prints(va_list, char*, uint64_t*);
void printx(va_list, char*, uint64_t*);
void printp(va_list, char*, uint64_t*);
int printf(char *format_specifier, ...)
{
    char buf[100] = { 0 };
    uint64_t length = 0;
    va_list ap;
    va_start(ap, format_specifier);
    while (*format_specifier != '\0')
    {
        if (*format_specifier == '\n')
        {
            buf[length] = '\n';
            length++;
            //      updateX_Y_for_New_Line();
        }
        if (*format_specifier == '%')
        {
            switch (*++format_specifier)
            {
                case 'd':
                    printd(ap, buf, &length);
                    break;
                case 'c':
                    buf[length] = va_arg(ap, int);
                    length++;
                    // add_character_to_screen( va_arg(ap,int));
                    break;
                case 's':
                    prints(ap, buf, &length);
                    break;
                case 'x':
                    //isHex = 1;
                    printx(ap, buf, &length);
                    break;
                case 'p':
                    printp(ap, buf, &length);
                    break;
            }
        }
        else
        {
            // add_character_to_screen(*format_specifier);
            // *pointer = *format_specifier;
            buf[length] = *format_specifier;
            length++;
            //  pointer++;
        }
        format_specifier++;
    }
    va_end(ap);
    __asm__ __volatile__( "movq $0x01, %%rbx":::"rbx");
    __asm__ __volatile__( "movq %0, %%rdx"::"g"(buf):"rdx", "memory");
    __asm__ __volatile__( "movq %0, %%rcx"::"g"(length):"rcx");
    //  __asm__ __volatile__( "  int $0x80");
    __asm__ __volatile__("int $0x80;":::"%rax","rdx","rbx","memory");
    return length;
}
void printd(va_list ap, char *arr, uint64_t *len)
{
    int p = va_arg(ap, int);
    if (p == 0)
    {
        arr[*len] = '0';
        *len = *len + 1;
        // add_character_to_screen('0');
        return;
    }
    if (p < 0)
    {
        arr[*len] = '-';
        *len = *len + 1;
        // add_character_to_screen('-');
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
        arr[*len] = a;
        *len = *len + 1;
        //    add_character_to_screen(a);
        temp = temp / 10;
    }
    while (tempP % 10 == 0)
    {
        char a = tempP % 10 + '0';
        arr[*len] = a;
        *len = *len + 1;
        //   add_character_to_screen(a);
        tempP = tempP / 10;
    }
}
void prints(va_list ap, char *arr, uint64_t *len)
{
    char *p = va_arg(ap, char *);
    while (*p != '\0')
    {
        arr[*len] = *p;
        *len = *len + 1;
        // add_character_to_screen(*p);
        p++;
    }
}
void printx(va_list ap, char* arr, uint64_t *len)
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
    arr[*len] = 0 + '0';
    *len = *len + 1;
    arr[*len] = 'x';
    *len = *len + 1;
    // add_character_to_screen((0+'0'));
    //add_character_to_screen(('x'));
    for (i = length - 1; i >= 0; i--)
    {
        switch (rem[i])
        {
            case 10:
                /*char a = 10 + '0';*/
                arr[*len] = 'a';
                *len = *len + 1;
                //	  add_character_to_screen(('a'));
                break;
            case 11:
                /*char b = 11 + '0';*/
                arr[*len] = 'b';
                *len = *len + 1;
                // add_character_to_screen(('b'));
                break;
            case 12:
                /* char c = 12 + '0';*/
                arr[*len] = 'c';
                *len = *len + 1;
                // add_character_to_screen(('c'));
                break;
            case 13:
                // char d = 13 + '0';
                arr[*len] = 'd';
                *len = *len + 1;
                // add_character_to_screen(('d'));
                break;
            case 14:
                //char e = 14 + '0';
                arr[*len] = 'e';
                *len = *len + 1;
                // add_character_to_screen(('e'));
                break;
            case 15:
                //char f = 15 + '0';
                arr[*len] = 'f';
                *len = *len + 1;
                // add_character_to_screen(('f'));
                break;
            default:
                //char rest = rem[i] + '0';
                arr[*len] = rem[i] + '0';
                *len = *len + 1;
                //  add_character_to_screen((rem[i]+'0'));
        }
    }
}
void printp(va_list ap, char *arr, uint64_t *len)
{
    // printx(ap);
    unsigned int num = va_arg(ap, unsigned int);
    int rem[50], i = 0, length = 0;
    while (num > 0)
    {
        rem[i] = num % 16;
        num = num / 16;
        i++;
        length++;
    }
    arr[*len] = 0 + '0';
    *len = *len + 1;
    arr[*len] = 'x';
    *len = *len + 1;
    //  add_character_to_screen((0+'0'));
    //  add_character_to_screen(('x'));
    for (i = length - 1; i >= 0; i--)
    {
        switch (rem[i])
        {
            case 10:
                /*char a = 10 + '0';*/
                arr[*len] = 'a';
                *len = *len + 1;
                //	  add_character_to_screen(('a'));
                break;
            case 11:
                /*char b = 11 + '0';*/
                arr[*len] = 'b';
                *len = *len + 1;
                // add_character_to_screen(('b'));
                break;
            case 12:
                /* char c = 12 + '0';*/
                arr[*len] = 'c';
                *len = *len + 1;
                // add_character_to_screen(('c'));
                break;
            case 13:
                // char d = 13 + '0';
                arr[*len] = 'd';
                *len = *len + 1;
                // add_character_to_screen(('d'));
                break;
            case 14:
                //char e = 14 + '0';
                arr[*len] = 'e';
                *len = *len + 1;
                // add_character_to_screen(('e'));
                break;
            case 15:
                //char f = 15 + '0';
                arr[*len] = 'f';
                *len = *len + 1;
                // add_character_to_screen(('f'));
                break;
            default:
                //char rest = rem[i] + '0';
                arr[*len] = rem[i] + '0';
                *len = *len + 1;
                //  add_character_to_screen((rem[i]+'0'));
        }
    }
}
