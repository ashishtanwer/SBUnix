#include<defs.h>
#include<stdio.h>
#include<inline.h>
#include<idt.h>
#include<handler.h>
#include<stdio.h>
#include<stdarg.h>
#include<process.h>
#include<vmm.h>
#include<scheduler.h>
#include<ahci.h>
#include<e1000.h>
extern uint64_t binary_start;
extern uint64_t binary_end;
char *pwd = "bin/";
void sys_setcurrentdir(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    char *filename = (char *) address;
    if (matchString3(filename, "..") == 0)
    {
        // printk("it is a .. ");
        char *temp_str = "bin";
        strcpy2(pwd, temp_str);
        int count = count_characters1(temp_str, '/');
        // printk("\n count ..%d",count);
        if (count == 0)
        {
            return;
        }
        int gotlength = count_length1(temp_str, '/', count - 1);
        char *get_v = get_till(temp_str, gotlength);
        strcpy2(get_v, pwd);
        return;
    }
    int is_direct = is_dir(filename, (char *) binary_start,
            (char *) binary_end);
    if (is_direct == 0)
    {
        filename = strcat(pwd, filename);
        int check = is_dir(filename, (char *) binary_start,
                (char *) binary_end);
        if (check == 0)
        {
            printk("\n Not a Directory");
        }
        else
        {
            strcpy2(filename, pwd);
        }
    }
    else
    {
        strcpy2(filename, pwd);
    }
}
void sys_readfile(struct register_frame *stack)
{
    uint64_t file_desc = stack->rdx;
    // printk("\n ssss: %x",file_desc);
    //  while(1);
    uint64_t *p = (uint64_t *) file_desc;
    uint64_t buffer_address = stack->r8;
    uint64_t *count = (uint64_t*) stack->r9;
    uint64_t return_address = stack->rcx;
    read_call(*p, buffer_address, *count, return_address);
    //  void read_call(uint64_t file_desc, uint64_t buffer_address,uint64_t count, uint64_t return_value)
}
void sys_closedirectory(struct register_frame *stack)
{
    uint64_t file_desc = stack->rdx;
    // printk("\n ssss: %x",file_desc);
    //  while(1);
    uint64_t *p = (uint64_t *) file_desc;
    uint64_t return_address = stack->rcx;
    close_dir_call(*p, return_address);
}
void sys_closefile(struct register_frame *stack)
{
    uint64_t file_desc = stack->rdx;
    // printk("\n ssss: %x",file_desc);
    //  while(1);
    uint64_t *p = (uint64_t *) file_desc;
    uint64_t return_address = stack->rcx;
    (*p, return_address);
}
void sys_opendirectory(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    // printk("Execute if : %x",address);
    //  printk("%s",filename);
    char *filename = (char *) address;
    uint64_t return_address = stack->rcx;
    open_dir_call(filename, return_address);
}
void sys_openfile(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    // printk("Execute if : %x",address);
    //  printk("%s",filename);
    char *filename = (char *) address;
    uint64_t return_address = stack->rcx;
    open_call(filename, return_address);
}
void sys_scanning(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    scan_process(address);
}
void sys_allocheap(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    uint64_t size = stack->rcx;
    //  printk("\n Size %d", size);
    // printk("\n address : %x",address);
    heap_allocation(size, address);
    //      heap_allocation(
}
void sys_execute(struct register_frame *stack)
{
    //char *filename = "bin/hello";
    uint64_t address = stack->rdx;
    // printk("Execute if : %x",address);
    //  printk("%s",filename);
    char *filename = (char *) address;
    //  printk("%s",filename);
    if (*filename == '\0')
    {
        printk("\n filename ");
        //  remove_process_yes();
        return;
    }
    copy_name(filename);
    sys_exec(filename, (char *) binary_start, (char *) binary_end);
    //  waiting_on_child();
}
void sys_waitingwithpid(struct register_frame *stack)
{
    uint64_t pid = stack->rdx;
    // printk("\n pid is : %d ",pid);
    waiting_on_pid(pid);
}
void sys_waiting(struct register_frame *stack)
{
    waiting_on_child();
}
void sys_forking(struct register_frame *stack)
{
    uint64_t address = stack->rdx;
    // printk("\n address : %x",address);
    sys_fork(address);
}
void sys_sleep(struct register_frame *stack)
{
    // uint64_t = (char*)stack->rdx;
    uint64_t time = stack->rcx;
    //  printk("\n Inside System call with time %d",time);
    //  printk("\n Address of it self %x",&sys_sleep);
    //printk("\n Address of go to sleep %x",&go_to_sleep);
    go_to_sleep(time);
    //      while(1);
}
void sys_write(struct register_frame *stack)
{
    printk("\n write System Call");
    // char *pointer = (char*)stack->rdx;
    //stack->rcx;
    // va_list p1 = *p;
    //    va_list* ap = (va_list*)stack->rcx;
    // va_copy(ap,(va_list*)stack->rcx);
    // print_user(pointer,ap);
}
void sys_brk(struct register_frame *stack)
{
    char *pointer = (char*) stack->rdx;
    int length = stack->rcx;
    //  printk("\n length :%d\n",length);
    // printk("\n legth %d\n",length);
    int i = 0;
    //    printk("\n");
    for (; i < length; i++)
    {
        if (*pointer == '\n')
        {
            printk("\n");
        }
        else
        {
            printk("%c", *pointer);
        }
        pointer++;
    }
}
void sys_exit(struct register_frame *stack)
{

    uint64_t status = stack->rdx;

    remove_process(status);
    while (1)
        ;
}
void sys_getcurrentdir(struct register_frame *stack)
{
    uint64_t buffer_address = stack->rdx;
    char *address = (char *) buffer_address;
    char *p = pwd;
    while (*p != '\0')
    {
        *address = *p;
        p++;
        address++;
    }
    *address = '\0';
}
void sys_psid(struct register_frame *stack)
{
    uint64_t return_address = stack->rdx;
    get_process_id(return_address);
}
void sys_printps(struct register_frame *stack)
{
    get_process_info();
}
void sys_readdirectorycall(struct register_frame *stack)
{
    uint64_t file_desc = stack->rdx;
    uint64_t *p = (uint64_t *) file_desc;
    uint64_t buffer_address = stack->r8;
    uint64_t return_address = stack->rcx;
    read_dir_call(*p, buffer_address, return_address);
}
