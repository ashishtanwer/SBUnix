#include<defs.h>
#include<stdio.h>
#include<inline.h>
#include<idt.h>
#include<handler.h>
#include<stdio.h>
#include<stdarg.h>
#include<process.h>
#include<syscall.h>
#include<vmm.h>
#include<scheduler.h>
#include<ahci.h>
#include<e1000.h>
#define MAX_INTERRUPTS 256
#define FLAG 0x8E
#define FLAG_USER 0xEE
extern struct task_struct * current;

static void *syscall[20] = { &sys_write, &sys_brk, &sys_exit, &sys_sleep, &sys_forking,
        &sys_waiting, &sys_waitingwithpid, &sys_execute, &sys_allocheap, &sys_scanning,
        &sys_openfile, &sys_closefile, &sys_readfile,
        &sys_opendirectory, &sys_closedirectory, &sys_printps,
        &sys_readdirectorycall, &sys_setcurrentdir, &sys_getcurrentdir, &sys_psid };
struct idt_descriptor
{
    uint16_t low_address_low_bit_ir;
    uint16_t selector;
    uint8_t reserved_bits;
    uint8_t precompiled_flag_bits;
    uint16_t low_address_high_bit_ir;
    uint32_t high_address_ir;
    uint32_t reserved1;
}__attribute__((packed));
struct idt_register
{
    uint16_t max_size_of_idt;
    uint64_t starting_address_of_idt;
}__attribute__((packed));
struct idt_descriptor _idt_descriptor[MAX_INTERRUPTS];
struct idt_register _idt_register;
void initialize_idt_register()
{
    //  printk("Initializing IDT Register Structure....\n");
    _idt_register.max_size_of_idt = sizeof(_idt_descriptor);
    // printk("max size of idt: %d",_idt_register.max_size_of_idt);
    _idt_register.starting_address_of_idt = (uint64_t) &_idt_descriptor[0];
    zero_out_space_for_idt();
    // printk("IDT Register Initialized.... \n");
}
void zero_out_space_for_idt()
{
    // printk("Zeroing out Space for IDT.... \n");
    int i = 0;
    char *temp_pointer = (char *) &_idt_descriptor[0];
    for (i = 0; i < _idt_register.max_size_of_idt; i++)
    {
        *temp_pointer = 0;
        temp_pointer++;
    }
    // printk("Space zeroed out....\n");
}
void load_idt_register()
{
    // printk("Loading idt register....\n");
    __asm__ __volatile__(
            "lidt (%0)"
            : /* no output */
            : "r" (&_idt_register)
    );
}
struct stack_frame_syscall* stack_sys;
void handler_systemcall_routine(struct stack_frame_syscall* stack)
{
    //  printk("\n stack rp : %d",stack->r9);
    stack_sys = stack;
    printk(" ");
    void *location = syscall[stack->rbx];
    //  printk("\n location: %x",location);
    /*
     __asm volatile (
     "call *%0; \n\t"
     :: "r" (location));
     */
    __asm volatile (" \
      pushq %1; \
      pushq %2; \
      pushq %3; \
      pushq %4; \
      pushq %5; \
      pushq %6; \
      movq %%rsp,%%rdi; \
      call *%7; \
      popq %%rbx;\
      popq %%rbx;\
      popq %%rbx;\
      popq %%rbx; \
      popq %%rbx; \
      popq %%rbx; \
    " :: "r" (stack->rdi), "r" (stack->rsi), "r" (stack->rdx), "r" (stack->rcx), "r" (stack->rax),"r"(stack->r8),"r"(stack->r9), "r" (location));
}
void handler_pagefault_routine(struct isr_pf_stack_frame *stack)
{
    uint64_t fault_address;
    uint64_t cr3;
    __asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    __asm volatile("mov %%cr3, %0" : "=r" (cr3));
    {
        printk("\n\n            cr3 registers  %x\n", cr3);
        printk("\n              %x\n", fault_address);
        printk("\n             p : %d\n", stack->error.error.p);
        printk("            wr : %d\n", stack->error.error.wr);
        printk("            us : %d\n", stack->error.error.us);
        printk("          rsvd : %d\n", stack->error.error.rsvd);
        printk("            CS : %x\n", stack->cs);
        printk("           RIP : %x\n", stack->rip);
        printk("           RSP : %x\n", stack->rsp);
        while (1)
            ;
    }
}
void handler_gp_routine(struct isr_pf_stack_frame *stack)
{
    uint64_t faultAddr;
    uint64_t cr3;
    __asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
    __asm volatile("mov %%cr3, %0" : "=r" (cr3));
    printk("\n            cr  %x\n", cr3);
    printk("\n              %x\n", faultAddr);
    printk("             p : %d\n", stack->error.error.p);
    //   printk("            id : %d\n", stack->error.error.id);
    printk("            wr : %d\n", stack->error.error.wr);
    printk("            us : %d\n", stack->error.error.us);
    printk("          rsvd : %d\n", stack->error.error.rsvd);
    printk("            CS : %x\n", stack->cs);
    printk("           RIP : %x\n", stack->rip);
    printk("           RSP : %x\n", stack->rsp);
    printk("\n It is a general protection fault");
    while (1)
        ;
}
void handler_default_routine()
{
    printk("This is a default handler\n");
    while (1)
        ;
}
void shiftPositionAndAddCharacter(char a)
{
    int previous_position_x = get_position_X();
    int previous_position_y = get_position_Y();
    set_position_X_Y(57, 24);
    printk("%c", a);
    set_position_X_Y(previous_position_x, previous_position_y);
}
void populate_idt_table(uint16_t segment_selector)
{
    initialize_idt_register();
    int i = 0;
    for (i = 0; i < MAX_INTERRUPTS; i++)
    {
        // initialize_idt_descriptor(i,8, 0, INTERRUPT , 0, ((uint64_t)&default_handler));
        init_idt(i, (uint64_t) &handler_default, segment_selector, 0, FLAG);
    }
    // printk("IDT table populated \n");
    init_idt(32, (uint64_t) &handler_timer, segment_selector, 0, FLAG);
    //  initialize_idt_descriptor(25,8, 0, INTERRUPT , 0, ((uint64_t)&handler_divide));
    init_idt(33, (uint64_t) &handler_keyboard, segment_selector, 0, FLAG);
    init_idt(13, (uint64_t) &handler_gp, segment_selector, 0,
    FLAG);
    init_idt(14, (uint64_t) &handler_pagefault, segment_selector, 0, FLAG);
    init_idt(42, (uint64_t) &handler_ahci, segment_selector, 0, FLAG);
    init_idt(43, (uint64_t) &handler_e1000, segment_selector, 0, FLAG);
    init_idt(128, (uint64_t) &handler_systemcall, segment_selector, 0,
            FLAG_USER);
    load_idt_register();
}
/*
 Code Referenced by :Chris Stone's shovelos - https://code.google.com/p/shovelos - New BSD License
 */
void init_idt(int interrupt_number, uint64_t interrupt_handler_address_64,
        uint16_t segment_selector, uint8_t reserved_bits, uint8_t flag_bits)
{
    _idt_descriptor[interrupt_number].low_address_low_bit_ir =
            (interrupt_handler_address_64) & 0x0000ffff;
    _idt_descriptor[interrupt_number].selector = segment_selector;
    _idt_descriptor[interrupt_number].reserved_bits = 0;
    _idt_descriptor[interrupt_number].precompiled_flag_bits = flag_bits;
    _idt_descriptor[interrupt_number].low_address_high_bit_ir =
            ((interrupt_handler_address_64) >> 16) & 0x0000ffff;
    _idt_descriptor[interrupt_number].high_address_ir =
            ((interrupt_handler_address_64) >> 32) & 0xffffffff;
    _idt_descriptor[interrupt_number].reserved1 = 0;
}
void strcpy2(const char *str1, char *str2)
{
    while (*str1 != '\0')
    {
        *str2 = *str1;
        str1++;
        str2++;
    }
    *str2 = '\0';
}
int strlen1(const char *str)
{
    int len = 0;
    for (len = 0; str[len]; len++)
        ;
    return len;
}
char pre[50] = { 0 };
char *
get_till(char *str1, int length)
{
    //  printk("\n sr: %s",str1);
    int i = 0;
    while (length > 0)
    {
        pre[i] = *str1;
        str1++;
        length--;
        i++;
    }
    pre[i] = '\0';
    return &pre[0];
}
char ret[50] = { 0 };
char*
strcat(char *str1, char *str2)
{
    int a = strlen1(str1);
    int b = strlen1(str2);
    // char *ret = (char*)malloc(a + b + 1);
    // char ret[50] ={0};
    int i = 0;
    for (int j = 0; j < a; i++, j++)
        ret[i] = str1[j];
    for (int j = 0; j < b; i++, j++)
        ret[i] = str2[j];
    ret[i] = '\0';
    return ret;
}
int matchString3(char *s, char *t)
{
    int ret = 0;
    while (!(ret = *(unsigned char *) s - *(unsigned char *) t) && *t)
    {
        ++s;
        ++t;
    }
    if (ret < 0)
    {
        ret = -1;
    }
    else if (ret > 0)
    {
        ret = 1;
    }
    return ret;
}
int count_characters1(const char *str, char character)
{
    const char *p = str;
    int count = 0;
    do
    {
        if (*p == character)
            count++;
    } while (*(p++));
    return count;
}
int count_length1(const char *str, char character, int count1)
{
    const char *p = str;
    int count = 0;
    int len = 0;
    do
    {
        len++;
        if (*p == character)
        {
            count++;
            if (count >= count1)
            {
                break;
            }
        }
    } while (*(p++));
    return len;
}
