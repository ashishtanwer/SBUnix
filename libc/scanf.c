#include<stdarg.h>
#include<stdio.h>
#include<defs.h>
int atoi(char*);
int scanf(const char *format_specifier, ...)
{
    char arr[100] = { 0 };
    // uint64_t ret = 100;
    __asm__ __volatile__ (
            "movq $0x9, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (arr):"rbx","rdx","memory");
    int length = 0;
    int *int_ptr = 0;
    char *char_ptr = 0;
    char *str_ptr = 0;
    // char  *point = (char*) &ret;
    char *point = arr;
    int result = 0;
    // while(*format_specifier != '\0'){
    //   char *sp = 0;
    va_list ap;
    va_start(ap, format_specifier);
    //  count =0;
    while (*format_specifier != '\0')
    {
        //	printk("In format specifier\n");
        if (*format_specifier == '%')
        {
            switch (*++format_specifier)
            {
                case 'd':
                    int_ptr = va_arg(ap, int *);
                    //	 result =  atoi((char *)&ret);
                    result = atoi(arr);
                    *int_ptr = result;
                    // *int_ptr = (int)(ret - '0');
                    // printd(ap);
                    break;
                case 'c':
                    char_ptr = va_arg(ap, char *);
                    // *char_ptr = (char)(ret);
                    *char_ptr = *arr;
                    //	 array[length1] = va_arg(ap,int);
                    // length1 = length1 + 1;
                    // add_character_to_screen( va_arg(ap,int));
                    break;
                case 's':
                    str_ptr = va_arg(ap, char *);
                    //	 sp = str_ptr;
                    while (*point != '\0')
                    {
                        //	   printk("%c",*point);
                        *str_ptr = *point;
                        str_ptr++;
                        length++;
                        point++;
                    }
                    *str_ptr = '\0';
                    break;
                case 'x':
                    //isHex = 1;
                    // printx(ap);
                    break;
                case 'p':
                    // printp(ap);
                    break;
            }
        }
        else
        {
        }
        format_specifier++;
    }
    va_end(ap);
    return 0;
}
int atoi(char *str)
{
    int res = 0; // Initialize result
    // Iterate through all characters of input string and update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
    // return result.
    return res;
}
