#include<defs.h>
#include<stdio.h>
#include<inline.h>
#include<idt.h>
#include<stdio.h>
#include<stdarg.h>
#include<process.h>
#include<vmm.h>
#include<scheduler.h>
#include<ahci.h>
#include<e1000.h>
#define MAX_INTERRUPTS 256
#define FLAG 0x8E
#define SCANCODE_SHIFT 42
#define FLAG_USER 0xEE
extern uint64_t binary_start;
extern uint64_t binary_end;
/*
 Code Referenced by :Chris Stone's shovelos - https://code.google.com/p/shovelos - New BSD License
 */
void handler_timer()
{
    __asm__(".global changing_registers1 \n"
            "changing_registers1:\n"
            "    cli;"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_timer_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "    mov $0x20, %al;"
            "    outb %al, $0x20;"
            "    outb %al, $0xA0;"
            "    sti;"
            "iretq;");
}

/*
 Code Referenced by : Chris Stone's shovelos - https://code.google.com/p/shovelos - New BSD License
 */
void handler_default()
{
    //   printingf("handling interrupt Number: ");
    __asm__(".global changing_registers \n"
            "changing_registers:\n"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_default_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "iretq;");
    // printingf("hello");
}
/*
 Code Referenced by :Chris Stone's shovelos - https://code.google.com/p/shovelos - New BSD License
 */
void handler_keyboard()
{
    //   printingf("handling interrupt Number: ");
    __asm__(".global changing_registers3 \n"
            "changing_resgisters3:\n"
            "    cli;"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_keyboard_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "    mov $0x20, %al;"
            "    outb %al, $0x20;"
            "    outb %al, $0xA0;"
            "    sti;"
            "iretq;");
}
void handler_pagefault()
{
    __asm__(".global changing_registers2 \n"
            "changing_resgisters2:\n"
            "    cli;"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_pagefault_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "    mov $0x20, %al;"
            "    outb %al, $0x20;"
            "    outb %al, $0xA0;"
            "    sti;"
            "iretq;");
}
void handler_systemcall()
{
    __asm__(".global changing_registers4 \n"
            "changing_resgisters4:\n"
            "    pushq %rbx;"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    call handler_systemcall_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "    popq %rbx;"
            "    mov $0x20, %al;"
            "    outb %al, $0x20;"
            "    outb %al, $0xA0;"
            "    sti;"
            "iretq;");
}
void handler_ahci()
{
    __asm__(".global handler_ahci \n"
            "handling_ahci_fault:\n"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_ahci_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "iretq;");
    // printingf("hello");
}
void handler_e1000()
{
    //   printingf("handling interrupt Number: ");
    __asm__(".global handling_e1000_fault \n"
            "handling_e1000_fault:\n"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_e1000_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "iretq;");
}
void handler_gp()
{
    __asm__(".global changing_registers7 \n"
            "changing_registers7:\n"
            "    pushq %rax;"
            "    pushq %rcx;"
            "    pushq %rdx;"
            "    pushq %rsi;"
            "    pushq %rdi;"
            "    pushq %r8;"
            "    pushq %r9;"
            "    pushq %r10;"
            "    pushq %r11;"
            "    movq  %rsp,%rdi;"
            "    addq  $72, %rdi;"
            "    call handler_gp_routine;"
            "    popq %r11;"
            "    popq %r10;"
            "    popq %r9;"
            "    popq %r8;"
            "    popq %rdi;"
            "    popq %rsi;"
            "    popq %rdx;"
            "    popq %rcx;"
            "    popq %rax;"
            "iretq;");
}
