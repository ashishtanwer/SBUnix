#ifndef _PROCESS_H
#define _PROCESS_H
#include<defs.h>

#define TASK_NEW 1
#define TASK_RUNNING 2
#define TASK_INTERRUPTABLE 3
#define TASK_UNINTERRUPTABLE 4
#define TASK_STOPPED 5
#define EXIT_ZOMBIE 6
#define MAX_PROCESS 100
#define TASK_WAITING 7
#define TASK_TERMINAL 9
#define TASK_TERMINAL_RUNNING 10

struct kernel_stack
{

    uint64_t stack_k[512];
};

struct user_stack
{

    uint64_t stack_k[512];
};

struct vm_area_struct
{

    uint64_t vm_start;
    uint64_t vm_end;
    uint64_t section_offset;
    struct vm_area_struct *vm_next;
    // uint64_t  vm_flags;
};

struct mm_struct
{

    /*
     uint64_t start_code;
     uint64_t end_code;
     uint64_t start_data;
     uint64_t statt_code;
     uint64_t start_brk;
     uint64_t brk;
     uint64_t mm_users;
     uint64_t mm_count;
     uint64_t stack_end;
     */
    struct vm_area_struct * mmap;
    struct vm_area_struct * current;
    uint64_t entryAddress;

};

struct task_struct
{

    uint64_t state;
    uint64_t processid;
    uint64_t cr3;
    uint64_t rsp;
    uint64_t timer;
    int wait_p;
    int status;
    uint64_t brk;

    char name[100];

    char scan_buf[100];

    uint64_t term_address;
    // uint64_t rip;

    int child_id[100];
    int child_length;

    int file_id[100];
    int file_length;

    int dir_id[100];
    int dir_length;

    //  struct task_struct *parent;
    // struct task_struct child[100];
    struct mm_struct *mm_struct_start;
    struct task_struct *task_next;

};

void get_process_id(uint64_t);

char *get_till(char *, int);

int count_length1(const char *, char, int);

int matchString3(char *, char *);

int strlen1(const char *);

char* strcat(char *, char *);

void strcpy1(const char *, char *);

void copy_name(char *);

void remove_process_yes();
void get_process_info();

int get_next_file(uint64_t, uint64_t);

void close_dir_call(uint64_t, uint64_t);

struct file_info * get_file_info(int);

void read_dir_call(uint64_t, uint64_t, uint64_t);

void read_call(uint64_t, uint64_t, uint64_t, uint64_t);

void close_call(uint64_t, uint64_t);

void open_dir_call(char *, uint64_t);

void open_call(char *, uint64_t);

int check_if_file_open(uint64_t);

int check_if_dir_open(uint64_t);

struct task_struct * get_current(int);

void scan_process(uint64_t);

void find_terminal_process(char *);

void heap_allocation(int, uint64_t);

int CopyVmaData(uint64_t);

void sys_exec(char *, char*, char*);

int waiting_on_pid(uint64_t);

void waiting_on_child();

int sys_fork(uint64_t);

void allocate_fork_page(uint64_t);

void memcpy(void *, void *, int);

void go_to_sleep(uint64_t);

struct mm_struct * copy_memory_regions();

void init_pcb();

void remove_process(uint64_t);

void init_process_creation();

void add_process(char *, char*, char*);

int getProcessId();

void schedule();
/*
 struct pcblist {

 struct task_struct pcb[MAX_PROCESS];
 uint8_t processid[MAX_PROCESS];
 };
 */

#endif
