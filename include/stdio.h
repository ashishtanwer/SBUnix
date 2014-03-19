#ifndef _STDIO_H
#define _STDIO_H

#include<stdarg.h>
//#include <unistd.h>

//int scanf();

int get_processid();

int setpwd(char *);
int getpwd(char *);

int is_dir(char *, char *, char*);
int readdir(int, char *);

int process();

int contains(const char *, const char *);

int count_characters(const char *, char);

int closedir(int);

int opendir(char*);

int read(int, char *, int);

int close(int);

int open(char *);

int execve(char *);

int wait();

int waitpid(int);

void clear_screen();

int printk(const char *format, ...);

int print_user(const char *format, va_list);

int sleep(int);

//int exit(int);

int fork();

void *malloc(int);

int scanf(const char *format, ...);

void starting_up_vga();

int get_position_X();

int get_position_Y();

void set_position_X_Y(int, int);

#endif
