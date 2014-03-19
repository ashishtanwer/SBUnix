#ifndef _IDT_H
#define _IDT_H

//#include <unistd.h>

//int printk(const char *format, ...);
//int scanf(const char *format, ...);
void init_timer();
void init_idt_register();
void zero_out_space_for_idt();
void populate_idt_table(uint16_t);
void init_idt(int, uint64_t, uint16_t, uint8_t, uint8_t);
void load_idt_register();
void handler_default();
void handler_keyboard();
void handler_timer_routine();
void handler_keyboard_routine();
void handler_timer();
void shiftPositionAndAddCharacter(char);
unsigned char get_other_characters(unsigned char);

struct isr_error_stack_frame
{
    uint64_t error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct isr_stack_frame
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct isr_pf_stack_frame
{
    union
    {

        uint64_t reserved;
        struct
        {
            unsigned p :1;
            unsigned wr :1;
            unsigned us :1;
            unsigned rsvd :1;
            unsigned id :1;
        } error;
    } error;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct stack_frame_syscall
{

    /*
     uint64_t r15;
     uint64_t r14;


     uint64_t r13;
     uint64_t r12;

     */
    uint64_t r11;

    uint64_t r10;
    uint64_t r9;
    uint64_t r8;

    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;

    uint64_t rax;
    uint64_t rbx;
    //  uint64_t reserved;
    /*
     uint64_t rax;
     uint64_t rcx;
     uint64_t rdx;
     uint64_t rsi;
     uint64_t rdi;
     */
    /*
     uint64_t r8;
     uint64_t r9;
     uint64_t r10;
     uint64_t r11;
     */
    /*
     union {
     uint64_t reserved;
     struct {
     unsigned p      : 1;
     unsigned wr     : 1;
     unsigned us     : 1;
     unsigned rsvd   : 1;
     unsigned id     : 1;
     }error;
     }error;
     */

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
    //  uint64_t rax;
};

struct register_frame
{

    uint64_t r9;
    uint64_t r8;
    uint64_t rax;
    // uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;

    //(->rdi), "r" (regs->rsi), "r" (regs->rdx), "r" (regs->rcx), "r" (regs->rbx), "r" (location)

};
#endif
