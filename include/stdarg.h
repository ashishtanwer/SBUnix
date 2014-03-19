#ifndef _STDARG_H
#define _STDARG_H

//#include <unistd.h>

/*
 void printingf(const char *format, ...);
 //int scanf(const char *format, ...);
 void starting_up_vga();

 int get_position_X();

 int get_position_Y();

 void set_position_XY(int,int);
 */

#define va_start(v,l)     __builtin_va_start(v,l)
#define va_arg(v,l)       __builtin_va_arg(v,l)
#define va_end(v)         __builtin_va_end(v)
#define va_copy(d,s)      __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

#endif

