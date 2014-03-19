#include<stdarg.h>
#include<defs.h>
#include<stdio.h>
int closedir(int file_desc)
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0xe, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"g"((uint64_t)&file_desc),"g"(&ret):"rbx","rdx","rcx","memory");
    return ret;
}
int close(int file_desc)
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0xb, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"g"((uint64_t)&file_desc),"g"(&ret):"rbx","rdx","rcx","memory");
    return ret;
}
int execve(char *p)
{
    int var = 7;
    __asm__ __volatile__("movq %0,%%rbx;"::"g"(var):"rbx","memory");
    __asm__ __volatile__("movq %0,%%rdx;"::"m"((uint64_t)p):"rdx","memory");
    __asm__ __volatile__("int $0x80;":::"%rax","rdx","rbx","memory");
    return 0;
}
void sys_exit(int status)
{
    int var = 2;
    __asm__ __volatile__("movq %0,%%rbx;"::"g"(var));
    uint64_t s = status;
    // int st = 4;
    __asm__ __volatile__("movq %0,%%rdx;"::"g"(s));
    __asm__ __volatile__("int $0x80;":::"%rax");
}
int fork()
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0x4, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx", "memory");
    return ret;
}
int get_processid()
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0x13, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"g" (&ret):"rbx","rdx","rcx","memory");
    return ret;
}
int getpwd(char *p)
{
    __asm__ __volatile__ (
            "movq $0x12, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"m"((uint64_t)p):"rbx","rdx","rcx","memory");
    return 0;
}
void* malloc(int size)
{
    uint64_t siz = size;
    uint64_t ret = 100;
    __asm__ __volatile__ (
            "movq $0x8, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"g" (&ret),"g"(siz):"rbx","rdx","rcx","memory");
    return (void*) ret;
}
int opendir(char *p)
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0xd, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"m"((uint64_t)p),"g" (&ret):"rbx","rdx","rcx","memory");


    return ret;

}
int open(char *p)
{
    int ret = 100;
    __asm__ __volatile__ (
            "movq $0xa, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"m"((uint64_t)p),"g" (&ret):"rbx","rdx","rcx","memory");
    return ret;

}
int process()
{

    __asm__ __volatile__ (
            "movq $0xf, %%rbx;\
               int $0x80;\
               ":::"rbx","rdx","rcx","memory");
    return 0;

}
int readdir(int desc, char *buff)
{

    int ret = 100;
    __asm__ __volatile__ (
            "movq $0x10, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               movq %2, %%r8;\
               int $0x80;\
               "::"g"(&desc),"g" (&ret),"m"((uint64_t)buff):"rbx","rdx","rcx","r8","r9","memory");

    return ret;

}
int read(int desc, char *buff, int bytes)
{


    int ret = 100;
    __asm__ __volatile__ (
            "movq $0xc, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               movq %2, %%r8;\
               movq %3, %%r9;\
               int $0x80;\
               "::"g"(&desc),"g" (&ret),"m"((uint64_t)buff),"g"(&bytes):"rbx","rdx","rcx","r8","r9","memory");


    return ret;

}
int setpwd(char *p)
{

    __asm__ __volatile__ (
            "movq $0x11, %%rbx;\
               movq %0, %%rdx;\
               int $0x80;\
               "::"m"((uint64_t)p):"rbx","rdx","rcx","memory");

    return 0;

}
int sleep(int seconds)
{
    int var = 3;
    uint64_t seconds2 = 2 * (uint64_t) seconds;
    __asm__ __volatile__("movq %0,%%rbx;"::"g"(var));
    __asm__ __volatile__("movq %0,%%rcx;"::"g"(seconds2));
    __asm__ __volatile__("int $0x80;":::"%rax");
    return 0;

}
int wait()
{
    int pid = -1;
    int status = 1000;
    __asm__ __volatile__ (
            "movq $0x5, %%rbx;\
               movq %0, %%rdx;\
               movq %1, %%rcx;\
               int $0x80;\
               "::"g" (&pid),"g"(&status):"rbx","rdx","rcx","memory");
    return 0;
}
int waitpid(int pid)
{
    int var = 6;
    __asm__ __volatile__("movq %0,%%rbx;"::"g"(var):"%rbx","memory");
    __asm__ __volatile__("movq %0,%%rdx;"::"g"((uint64_t)pid):"rdx","memory");
    __asm__ __volatile__("int $0x80;":::"%rax","%rbx","rdx","memory");
    return 0;

}
