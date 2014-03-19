#include<defs.h>
#include<stdio.h>
#include<pmm.h>
#include<process.h>
#include<vmm.h>
#include<scheduler.h>
#include<gdt.h>
#include<tarfs.h>
#include<idt.h>
#define MAX_PROCESS 100
uint64_t COW1 = 0x1000000000000;
uint8_t process_id[100];
struct task_struct * current;
struct task_struct * first;
extern struct stack_frame_syscall* stack_sys;
extern uint64_t binary_start;
extern uint64_t binary_end;
uint64_t userStackAddr = 0x200000;
uint64_t heap_address = 0x100000;
uint64_t *stack;
uint64_t *heap;
uint64_t curStack;
void init_pcb()
{
    int i;
    for (i = 0; i < MAX_PROCESS; i++)
    {
        process_id[i] = 0;
    }
}
int getProcessId()
{
    int i;
    for (i = 1; i < MAX_PROCESS; i++)
    {
        if (process_id[i] == 0)
        {
            process_id[i] = 1;
            return i;
        }
    }
    return -1;
}
uint64_t rsssp;
void init_function()
{
    while (1)
        ;
}
void traverse(struct mm_struct *mm_struct)
{
    struct vm_area_struct * start = mm_struct->mmap;
    while (start)
    {
        start = start->vm_next;
    }
}
/*
 Code Reference :  My Previous Submission
 */
int sys_fork(uint64_t address)
{
    struct task_struct *childProcess = (struct task_struct *) virtual_alloc();
    //  while(1);
    int processid = getProcessId();
    // printk("\n Process Id %d",processid);
    childProcess->processid = processid;
    childProcess->state = TASK_NEW;
    childProcess->brk = current->brk;
    childProcess->wait_p = -1;
    current->child_id[current->child_length] = processid;
    current->child_length = current->child_length + 1;
    //  childProcess->cr3=  (uint64_t)user_memory_setup();
    childProcess->cr3 = (uint64_t) fork_paging();
    // printk("\n childProcess cr3: %x",childProcess->cr3);
    //     while(1);
    struct PML4 *current = (struct PML4 *) get_Cr3();
    // printk("\n Current Cr3 : %x",current);
    //    while(1);
    SetCr3((struct PML4 *) childProcess->cr3);
    //  while(1);
    __asm volatile("movq %%rsp, %0;":"=g"(curStack));
    void * kernel_stack = (void *) virtual_alloc();
    childProcess->rsp = (uint64_t) kernel_stack;
    int *p = (int *) address;
    *p = 0;
    __asm volatile( "movq %0, %%rsp ": : "m"(childProcess->rsp) : "memory" );
    __asm volatile("pushq $0x23\n\t"
            "pushq %0\n\t"
            "pushq $0x200\n\t"
            "pushq $0x1b\n\t"
            "pushq %1\n\t"
            : :"m"(stack_sys->rsp),"m"(stack_sys->rip) :"memory");
    __asm volatile("pushq %0\n\t"
            "pushq %1\n\t"
            "pushq %2\n\t"
            "pushq %3\n\t"
            "pushq %4\n\t"
            "pushq %5\n\t"
            "pushq %6\n\t"
            "pushq %7\n\t"
            "pushq %8\n\t"
            : :"m"(stack_sys->rbx),"m"(stack_sys->rcx),"m"(stack_sys->rdx),"m"(stack_sys->rsi),"m"(stack_sys->rdi),"m"(stack_sys->r8),"m"(stack_sys->r9),"m"(stack_sys->r10),"m"(stack_sys->r11):"memory");
    __asm volatile("movq %%rsp, %0;":"=g"(childProcess->rsp) : :"memory");
    __asm volatile( "movq %0, %%rsp ": : "m"(curStack) : "memory" );
    insert(childProcess);
    SetCr3(current);
    int *p1 = (int *) address;
    *p1 = childProcess->processid;
    return 0;
}
/*
 Code Reference :  My Previous Submission
 */
void sys_exec(char *fileName, char *binary_tarfs_start, char* binary_tarfs_end)
{
    int value = get_elf_file(&_binary_tarfs_start, fileName,
            &_binary_tarfs_end);
    if (value)
    {
        struct mm_struct *mm_struct = (struct mm_struct *) virtual_alloc();
        current->mm_struct_start = mm_struct;
        stack = (uint64_t*) AssignPhysicalToVirtual(userStackAddr, 1);
        struct vm_area_struct* vma_user_stack =
                (struct vm_area_struct *) virtual_alloc();
        vma_user_stack->vm_start = userStackAddr;
        vma_user_stack->vm_end = userStackAddr + 0x1000;
        vma_user_stack->section_offset = -1;
        mm_struct->mmap = vma_user_stack;
        mm_struct->current = vma_user_stack;
        mm_struct->current->vm_next = NULL;
        map_exe(mm_struct);
        __asm volatile("pushq $0x23\n\t"
                "pushq %0\n\t"
                "pushq $0x200\n\t"
                "pushq $0x1b\n\t"
                "pushq %1\n\t"
                : :"c"((uint64_t)(&stack[500])),"d"(mm_struct->entryAddress) :"memory");
        __asm volatile( "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                : : :"memory");
        __asm volatile("popq %%r11\n\t"
                "popq %%r10\n\t"
                "popq %%r9\n\t"
                "popq %%r8\n\t"
                "popq %%rdi\n\t"
                "popq %%rsi\n\t"
                "popq %%rdx\n\t"
                "popq %%rcx\n\t"
                "popq %%rbx\n\t"
                : : :"memory");
        __asm volatile("iretq":::"memory");
    }
    else
    {
        printk("\n command not found");
        remove_process(0);
    }
}
/*
 Code Reference :  My Previous Submission
 */
void add_process(char *fileName, char *binary_tarfs_start,
        char* binary_tarfs_end)
{
    int value = get_elf_file(&_binary_tarfs_start, fileName,
            &_binary_tarfs_end);
    if (value)
    {
        //  printk("\n Inside Add Process");
        int processid = getProcessId();
        // create new Process
        struct task_struct *newProcess = (struct task_struct *) virtual_alloc();
        // Give a process id
        newProcess->processid = processid;
        newProcess->state = TASK_NEW;
        newProcess->wait_p = -1;
        strcpy1(fileName, newProcess->name);
        newProcess->timer = 0;
        // Identity Map Kernel
        newProcess->cr3 = (uint64_t) user_memory_setup();
        // printk("New Process %x\n", newProcess->cr3);
        // Get Current CR3
        struct PML4 *curPML4 = (struct PML4 *) get_Cr3();
        // Replace  with our CR3
        SetCr3((struct PML4 *) newProcess->cr3);
        // Start Creating mm_struct
        struct mm_struct *mm_struct = (struct mm_struct *) virtual_alloc();
        newProcess->mm_struct_start = mm_struct;
        // Get the RSP of the previous stack
        __asm volatile("movq %%rsp, %0;":"=g"(curStack));
        void * kernel_stack = (void *) virtual_alloc();
        // In the new CR3 add the kernel stack info and get a page for it
        //  setup_kernel_stack();
        newProcess->rsp = (uint64_t) kernel_stack;
        // Change it with our rsp
        // newProcess->rsp =  KERNEL_STACK_START_VIRTUAL_ADDRESS;
        __asm volatile( "movq %0, %%rsp ": : "m"(newProcess->rsp) : "memory" );
        stack = (uint64_t*) AssignPhysicalToVirtual(userStackAddr, 1);
        heap = (uint64_t *) AssignPhysicalToVirtual(heap_address, 1);
        //setup_user_stack();
        struct vm_area_struct* vma_user_stack =
                (struct vm_area_struct *) virtual_alloc();
        vma_user_stack->vm_start = userStackAddr;
        vma_user_stack->vm_end = userStackAddr + 0x1000;
        vma_user_stack->section_offset = -1;
        mm_struct->mmap = vma_user_stack;
        mm_struct->current = vma_user_stack;
        mm_struct->current->vm_next = NULL;
        struct vm_area_struct* vma_heap =
                (struct vm_area_struct *) virtual_alloc();
        vma_heap->vm_start = heap_address;
        vma_heap->vm_end = heap_address + 0x1000;
        vma_heap->section_offset = -1;
        mm_struct->current->vm_next = vma_heap;
        mm_struct->current = vma_heap;
        mm_struct->current->vm_next = NULL;
        newProcess->brk = heap_address;
        map_exe(mm_struct);
        __asm volatile("pushq $0x23\n\t"
                "pushq %0\n\t"
                "pushq $0x200\n\t"
                "pushq $0x1b\n\t"
                "pushq %1\n\t"
                : :"c"((uint64_t)(&stack[500])),"d"(mm_struct->entryAddress) :"memory");
        __asm volatile( "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                "pushq $0x0\n\t"
                : : :"memory");
        __asm volatile("movq %%rsp, %0;":"=g"(newProcess->rsp) : :"memory");
        __asm volatile( "movq %0, %%rsp ": : "m"(curStack) : "memory" );
        insert(newProcess);
        SetCr3(curPML4);
    }
    else
    {
        printk("\n Donot have file ...");
    }
}
void init_process_creation()
{
    struct task_struct * init = (struct task_struct *) virtual_alloc();
    // printk("\n init %x",init);
    init->state = TASK_NEW;
    init->processid = 0;
    init->task_next = NULL;
    init->mm_struct_start = NULL;
    // init->parent = NULL;
    init->cr3 = get_Cr3();
    init->timer = 0;
    init->wait_p = -1;
    void * kernel_stack = (void *) virtual_alloc();
    init->rsp = (uint64_t) kernel_stack;
    //  init->rsp =  KERNEL_STACK_START_VIRTUAL_ADDRESS;
    first = init;
    current = init;
    int a = 0x2B;
    __asm volatile("movq %0,%%rax;\n\t"
            "ltr (%%rax);"::"r"(&a));
    __asm volatile("movq %%rsp, %0;":"=g"(curStack));
    __asm volatile( "movq %0, %%rsp ": : "m"(init->rsp) : "memory" );
    __asm volatile("movq %%rsp, %0;":"=g"(init->rsp));
    __asm volatile( "movq %0, %%rsp ": : "m"(curStack) : "memory" );
    init_process(init);
}
/*
 Code Reference :  My Previous Submission
 */
void schedule()
{
    struct task_struct * task = get_next_process();
    if (current->processid == task->processid)
    {
        return;
    }
    if (task->processid == 0)
    {
        __asm__( "    mov $0x20, %al;"
                "    outb %al, $0x20;"
                "    outb %al, $0xA0;"
                "    sti;");
        __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
        __asm volatile("movq %0, %%cr3":: "a"(task->cr3));
        // __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
        //    while(1);
        // Moving the stack pointer
        //    printk("\n Rsp Task : %x",task->rsp);
        __asm volatile( "movq %0, %%rsp ": : "m"(task->rsp) : "memory" );
        //    tss.rsp0 = (uint64_t)task->rsp;
        current = task;
        __asm__ __volatile__("jmp %0"::"r"(&init_function));
    }
    else
    {
        if (task->state == TASK_NEW)
        {
            task->state = TASK_RUNNING;
            // printk("\n Found a brand new  Process");
            //  printk("\n What...");
            //    while(1);
            __asm__( "    mov $0x20, %al;"
                    "    outb %al, $0x20;"
                    "    outb %al, $0xA0;"
                    "    sti;");
            __asm volatile("movq %0, %%cr3":: "a"(task->cr3));
            __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
            current = task;
            // printk("\n Current RSP: %x", current->rsp);
            __asm volatile( "movq %0, %%rsp ": : "m"(task->rsp) : "memory" );
            //	    uint64_t rsp2 ;
            __asm__ __volatile__("movq %%rsp,%0;":"=g"(tss.rsp0));
            __asm volatile("popq %%r11\n\t"
                    "popq %%r10\n\t"
                    "popq %%r9\n\t"
                    "popq %%r8\n\t"
                    "popq %%rdi\n\t"
                    "popq %%rsi\n\t"
                    "popq %%rdx\n\t"
                    "popq %%rcx\n\t"
                    "popq %%rbx\n\t"
                    : : :"memory");
            __asm volatile("iretq":::"memory");
        }
        else if (task->state == TASK_RUNNING)
        {
            __asm volatile("movq %0, %%cr3":: "a"(task->cr3));
            //     while(1);
            __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
            // printk("\n returning rsp %x ",current->rsp);
            __asm volatile( "movq %0, %%rsp ": : "m"(task->rsp) : "memory" );
            tss.rsp0 = (uint64_t) task->rsp;
            // tss.rsp0 = KERNEL_STACK_START_VIRTUAL_ADDRESS;
            current = task;
        }
        else if (task->state == TASK_TERMINAL)
        {
            __asm volatile("movq %0, %%cr3":: "a"(task->cr3));
            //     while(1);
            __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
            __asm volatile( "movq %0, %%rsp ": : "m"(task->rsp) : "memory" );
            tss.rsp0 = (uint64_t) task->rsp;
            //     printk("\n Copying to address ...");
            char * p = task->scan_buf;
            char *z = (char *) task->term_address;
            //     int length3  = 0;
            while (*p != '\0')
            {
                //         printk("%c",*p);
                *z = *p;
                p++;
                z++;
                //length3++;
            }
            *z = *p;
            // printk("\n Legrhrhr %d", length3);
            // while(1);
            //     *z='\0';
            current = task;
            // printk("\n swithc it ");
            //  while(1);
        }
    }
    //printk("\n Going Out...");
    // while(1);
}
void remove_process_yes()
{
    current->state = EXIT_ZOMBIE;
    remove(current);
    // insert_zombie_queue(current);
    schedule();
}
void remove_process(uint64_t status)
{
    current->status = status;
    current->state = EXIT_ZOMBIE;
    //    while(1);
    // __asm volatile( "movq %0, %%rsp ": : "m"(current->rsp) : "memory" );
    remove(current);
    insert_zombie_queue(current);
    //  while(1);
    schedule();
}
void go_to_sleep(uint64_t time)
{
    //  __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
    int processid = current->processid;
    // printk("\n updating timer ...");
    current->timer = time;
    // printk("\n Going to Sleep ...");
    // current->state = TASK_WAITING;
    insert_wait_queue(current);
    // printk("\n inserted ..");
    remove(current);
    // printk("\n removed ..");
    // __asm__ __volatile__("movq %%rsp,%0;":"=g"(current->rsp));
    current = get_current(processid);
    schedule();
    // printk("\n scheduling ");
}
void get_process_info()
{
    print_process();
}
void strcpy1(const char *str1, char *str2)
{
    while (*str1 != '\0')
    {
        *str2 = *str1;
        str1++;
        str2++;
    }
    *str2 = '\0';
}
void copy_name(char *filename)
{
    //  int processid = current->processid;
    //current =  get_current(processid);
    strcpy1(filename, current->name);
    // printk("\n %s ",current->name);
    //  get_current_run
}
void get_process_id(uint64_t return_value)
{
    int * ret = (int *) return_value;
    // int i ;
    *ret = (int) current->processid;
}
void waiting_on_child()
{
    int processid = current->processid;
    current->state = TASK_WAITING;
    insert_wait_queue(current);
    remove(current);
    current = get_current(processid);
    schedule();
}
void scan_process(uint64_t address)
{
    int processid = current->processid;
    current->state = TASK_TERMINAL;
    current->term_address = address;
    insert_wait_queue(current);
    //  current =  get_current(processid);
    remove(current);
    current = get_current(processid);
    schedule();
}
/*
 Code Reference :  My Previous Submission
 */
void read_dir_call(uint64_t file_desc, uint64_t buffer_address,
        uint64_t return_value)
{
    char * mem_file = (char *) buffer_address;
    int * ret = (int *) return_value;
    int i;
    // printk("\n desc %d",file_desc);
    // printk("\n %d",current->file_length);
    if ((int) file_desc == -1)
    {
        *ret = 0;
        *mem_file = '\0';
        return;
    }
    for (i = 0; i < current->dir_length; i++)
    {
        //    printk("\n current fileid %d",current->file_id[i]);
        if (current->dir_id[i] == (int) file_desc)
        {
            //  printk("\n current fileid %d",current->file_id[i]);
            break;
        }
    }
    if (i == current->dir_length)
    {
        // printk("\n descriptor not present");
        // int * ret = (int *) return_value;
        *ret = 0;
        *mem_file = '\0';
        return;
    }
    struct dir_info* dir = (struct dir_info *) get_dir_info((int) file_desc);
    if (dir->end_of_dir == 1)
    {
        *mem_file = '\0';
        *ret = 0;
        return;
    }
    int no_more_files = 0;
    while (1)
    {
        int result = get_next_file(dir->dir_current, dir->dir_start);
        if (result == 1)
        {
            *mem_file = '\0';
            dir->end_of_dir = 1;
            no_more_files = 1;
            //printk("No more files ");
            break;
        }
        else if (result == 2)
        {
            // get the next address of the file
            dir->dir_current = get_next_pointer(dir->dir_current);
        }
        else if (result == 3)
        {
            dir->dir_current = get_next_pointer(dir->dir_current);
            break;
        }
    }
    if (no_more_files == 1)
    {
        *ret = 0;
        return;
    }
    char *name_of_file = get_name_file(dir->dir_current);
    while (*name_of_file != '\0')
    {
        *mem_file = *name_of_file;
        mem_file++;
        name_of_file++;
    }
    *mem_file = '\0';
    *ret = 1;
}
/*
 Code Reference :  My Previous Submission
 */
void read_call(uint64_t file_desc, uint64_t buffer_address, uint64_t count,
        uint64_t return_value)
{
    int bytes = (uint64_t) count;
    count = 40;
    int * ret = (int *) return_value;
    int i;
    // printk("\n desc %d",file_desc);
    // printk("\n %d",current->file_length);
    for (i = 0; i < current->file_length; i++)
    {
        //    printk("\n current fileid %d",current->file_id[i]);
        if (current->file_id[i] == (int) file_desc)
        {
            //  printk("\n current fileid %d",current->file_id[i]);
            break;
        }
    }
    if (i == current->file_length)
    {
        // printk("\n descriptor not present");
        // int * ret = (int *) return_value;
        *ret = -1;
        return;
    }
    struct file_info* file = (struct file_info *) get_file_info(
            (int) file_desc);
    char * disk_file = (char *) file->file_current;
    char * mem_file = (char *) buffer_address;
    if (file->end_of_file == 1)
    {
        *mem_file = '\0';
        *ret = 0;
        return;
    }
    for (; count > 0; count--)
    {
        if (*disk_file == '\0')
        {
            // printk("\n end of file reached..");
            file->file_current = (uint64_t) disk_file;
            file->end_of_file = 1;
            *mem_file = '\0';
            break;
        }
        *mem_file = *disk_file;
        mem_file = mem_file + 1;
        disk_file = disk_file + 1;
    }
    file->file_current = (uint64_t) disk_file;
    *mem_file = '\0';
    bytes = bytes - count;
    *ret = bytes;
}
void close_dir_call(uint64_t file_desc, uint64_t return_value)
{
    int * ret = (int *) return_value;
    int i;
    for (i = 0; i < current->dir_length; i++)
    {
        if (current->dir_id[i] == (int) file_desc)
        {
            break;
        }
    }
    if (i == current->dir_length)
    {
        *ret = -1;
        return;
    }
    int c;
    for (c = i; c < (current->dir_length) - 1; c++)
        current->dir_id[c] = current->dir_id[c + 1];
    current->dir_length = current->dir_length - 1;
    *ret = 0;
}
void close_call(uint64_t file_desc, uint64_t return_value)
{
    int * ret = (int *) return_value;
    int i;
    for (i = 0; i < current->file_length; i++)
    {
        if (current->file_id[i] == (int) file_desc)
        {
            break;
        }
    }
    if (i == current->file_length)
    {
        *ret = -1;
        return;
    }
    int c;
    for (c = i; c < (current->file_length) - 1; c++)
        current->file_id[c] = current->file_id[c + 1];
    current->file_length = current->file_length - 1;
    //  descriptor->count = descriptor->count -1;
    *ret = 0;
}
void open_call(char *filename, uint64_t return_value)
{
    int id = -1;
    uint64_t address = get_address_file(filename, (char *) binary_start,
            (char*) binary_end);
    // printk("%x",address);
    if (address != 0)
    {
        int is_present = check_if_file_open(address);
        if (is_present == -1)
        {
            id = open_file(filename);
            current->file_id[current->file_length] = id;
            current->file_length = current->file_length + 1;
        }
        else
        {
            id = is_present;
        }
    }
    int * ret = (int *) return_value;
    *ret = id;
}
void open_dir_call(char *filename, uint64_t return_value)
{
    int id = -1;
    uint64_t address = get_address_dir(filename, (char *) binary_start,
            (char*) binary_end);
    // printk("%x",address);
    if (address != 0)
    {
        int is_present = check_if_dir_open(address);
        if (is_present == -1)
        {
            id = open_dir(filename);
            current->dir_id[current->dir_length] = id;
            current->dir_length = current->dir_length + 1;
        }
        else
        {
            id = is_present;
        }
    }
    int * ret = (int *) return_value;
    *ret = id;
}
int check_if_dir_open(uint64_t address)
{
    int is_present = -1;
    int i = 0;
    for (i = 0; i < current->dir_length; i++)
    {
        uint64_t present_address = get_dir_start_address(current->dir_id[i]);
        if (present_address == address)
        {
            return current->dir_id[i];
        }
    }
    return is_present;
}
int check_if_file_open(uint64_t address)
{
    int is_present = -1;
    int i = 0;
    for (i = 0; i < current->file_length; i++)
    {
        uint64_t present_address = get_file_start_address(current->file_id[i]);
        if (present_address == address)
        {
            return current->file_id[i];
        }
    }
    return is_present;
}
int waiting_on_pid(uint64_t pid)
{
    int processid = current->processid;
    current->state = TASK_WAITING;
    current->wait_p = pid;
    insert_wait_queue(current);
    remove(current);
    current = get_current(processid);
    schedule();
    return 0;
}
void heap_allocation(int size, uint64_t address)
{
    // printk("\n heap address %x", heap_address);
    if ((current->brk + size) >= heap_address + 0x1000)
    {
        uint64_t *p = (uint64_t *) address;
        *p = NULL;
        // return null
    }
    else
    {
        // return current_brk
        uint64_t *p = (uint64_t *) address;
        *p = current->brk;
        current->brk = current->brk + size;
    }
}
/*
 Code Reference :  My Previous Submission
 */
struct mm_struct * copy_memory_regions()
{
    struct mm_struct* current_mm_struct = current->mm_struct_start;
    struct mm_struct* new_mm_struct = (struct mm_struct*) virtual_alloc();
    printk("\n new_mm_struct %x", new_mm_struct);
    new_mm_struct->entryAddress = current_mm_struct->entryAddress;
    struct vm_area_struct* current_vma = current_mm_struct->mmap;
    struct vm_area_struct * newvma_first = NULL;
    while (current_vma != NULL)
    {
        if (newvma_first == NULL)
        {
            struct vm_area_struct* newvma =
                    (struct vm_area_struct *) virtual_alloc();
            newvma->vm_start = current_vma->vm_start;
            newvma->vm_end = current_vma->vm_end;
            newvma->vm_next = NULL;
            // printk("\n New VMA created");
            newvma_first = newvma;
            new_mm_struct->mmap = newvma_first;
            current_vma = current_vma->vm_next;
        }
        else
        {
            struct vm_area_struct* newvma =
                    (struct vm_area_struct *) virtual_alloc();
            newvma->vm_start = current_vma->vm_start;
            newvma->vm_end = current_vma->vm_end;
            newvma->vm_next = NULL;
            newvma_first->vm_next = newvma;
            newvma_first = newvma;
            current_vma = current_vma->vm_next;
        }
    }
    return new_mm_struct;
}
/*
 Code Reference :  My Previous Submission
 */
void allocate_fork_page(uint64_t virtual_address)
{
    struct mm_struct * current_mm_struct = current->mm_struct_start;
    struct vm_area_struct * first_vma = current_mm_struct->mmap;
    // printk("\n inside allocate ");
    while (first_vma != NULL)
    {
        if (first_vma->vm_start <= virtual_address
                && first_vma->vm_end > virtual_address)
        {
            uint64_t number_of_pages = (first_vma->vm_end - first_vma->vm_start)
                    / 0x1000;
            int i = 0;
            uint64_t pt_entry = get_physical_address(first_vma->vm_start);
            uint64_t pt_entry_cow = (pt_entry & ~COW1) & 0xFFFFFFFFFFFFF000;
            // printk("\n pt entry withour cow %x",pt_entry_cow);
            uint64_t temp = (uint64_t) first_vma->vm_start;
            for (; i < number_of_pages; i++)
            {
                //	while(1);
                allocation_and_map(temp);
                //	while(1);
                printk("\n temp : %x", temp);
                int size = 4096;
                char* phyStart = (char*) (IDENTITY_MAP_VIRTUAL_START
                        | pt_entry_cow);
                char* addr = (char*) temp;
                while (size--)
                    *addr++ = *phyStart++;
                pt_entry_cow = pt_entry_cow + 0x1000;
                temp = temp + 0x1000;
            }
            break;
        }
        //printk("\n going to next vma");
        first_vma = first_vma->vm_next;
    }
}
/*
 Code Reference :  My Previous Submission
 */
int CopyVmaData(uint64_t virtAddr)
{
    uint64_t base = IDENTITY_MAP_VIRTUAL_START;
    struct mm_struct *memStr = (struct mm_struct *) current->mm_struct_start;
    int success = 0;
    if (memStr != NULL)
    {
        struct vm_area_struct* curVma = memStr->mmap;
        while (curVma != NULL)
        {
            if (curVma->vm_start <= virtAddr && virtAddr <= curVma->vm_end)
            {
                uint64_t PTentry = get_physical_address(curVma->vm_start)
                        & 0xFFFFFFFFFFFFF000;
                //Create a new Physical Page fr this VMA into which we will copy the contents from the old physical address it was pointing at
                AssignPhysicalToVirtual(curVma->vm_start, 1);
                memcpy((void *) (base | (PTentry & ~COW1)),
                        (void *) (curVma->vm_start), 4096);
                success = 1;
                break;
            }
            curVma = curVma->vm_next;
        }
    }
    return success;
}
