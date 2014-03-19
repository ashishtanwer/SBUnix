#include<stdio.h>
#include<defs.h>
#include<tarfs.h>
#include<elf.h>
#include<pmm.h>
#include<vmm.h>
#include<gdt.h>
#include<process.h>

struct file_info file_info_arr[100];
struct dir_info dir_info_arr[100];
uint8_t dir_id[100];
uint8_t file_id[100];
extern uint64_t binary_start;
extern uint64_t binary_end;
struct Exe_Format exeFormat;
programHeader pdr;
uint64_t elf_start;
void init_tarfs()
{
    int i;
    for (i = 0; i < MAX_PROCESS; i++)
    {
        file_id[i] = 0;
    }
    for (i = 0; i < MAX_PROCESS; i++)
    {
        dir_id[i] = 0;
    }
}
int open_dir(char * filename)
{
    int value = read_tarfs_file(filename, (char*) binary_start,
            (char *) binary_end);
    if (value == 0)
    {
        return -1;
    }
    int id = get_dir_id();
    if (id == -1)
    {
        return -1;
    }
    struct dir_info dir_info = dir_info_arr[id];
    dir_info.end_of_dir = 0;
    dir_info.dir_start = -1;
    dir_info.dir_current = -1;
    //  dir_info.previous_file = '\0';
    uint64_t address = get_address_dir(filename, (char *) binary_start,
            (char*) binary_end);
    if (address != 0)
    {
        dir_info.dir_start = address;
        dir_info.dir_current = address;
        strcpy(filename, dir_info.dir_path);
        //printk("\n dirname :%s",dir_info.dir_path);
        strcpy(filename, dir_info.previous_file);
        // printk("\n dirname :%s",dir_info.dir_path);
        dir_info_arr[id] = dir_info;
        return id;
    }
    else
    {
        return -1;
    }
}
void strcpy(const char *str1, char *str2)
{
    while (*str1 != '\0')
    {
        *str2 = *str1;
        str1++;
        str2++;
    }
    *str2 = '\0';
}
int open_file(char *filename)
{
    int value = read_tarfs_file(filename, (char *) binary_start,
            (char *) binary_end);
    if (value == 0)
    {
        return -1;
    }
    int id = get_file_id();
    if (id == -1)
    {
        return -1;
    }
    struct file_info file_info = file_info_arr[id];
    file_info.file_start = -1;
    file_info.file_current = -1;
    file_info.end_of_file = 0;
    uint64_t address = get_address_file(filename, (char *) binary_start,
            (char*) binary_end);
    if (address != 0)
    {
        file_info.file_start = address;
        file_info.end_of_file = 0;
        file_info.file_current = address;
        file_info_arr[id] = file_info;
        return id;
    }
    else
    {
        return -1;
    }
}
struct file_info* get_file_info(int fileid)
{
    return &file_info_arr[fileid];
}
uint64_t get_file_start_address(int fileid)
{
    return file_info_arr[fileid].file_start;
}
struct dir_info* get_dir_info(int fileid)
{
    return &dir_info_arr[fileid];
}
uint64_t get_dir_start_address(int fileid)
{
    return dir_info_arr[fileid].dir_start;
}
uint64_t get_address_dir(char * fileName, char *binary_tarfs_start,
        char* binary_tarfs_end)
{
    // printk("Indisde : %s",fileName);
    uint64_t file_address = 0;
    struct posix_header_ustar *tar_p =
            (struct posix_header_ustar *) (binary_tarfs_start);
    // print_posix_header(tar_p);
    //printk("\n Binary Start  %x", binary_tarfs_start);
    // printk("\n Binary End    %x", binary_tarfs_end);
    char *temp = binary_tarfs_start;
    // int size = octal_decimal(atoi(tar_p->size));
    while (temp < binary_tarfs_end)
    {
        if (matchString(tar_p->name, fileName) == 0)
        {
            int size = octal_decimal(atoi(tar_p->size));
            // printk("");
            if (size > 0)
            {
                return file_address;
            }
            file_address = (uint64_t) (temp);
            return file_address;
            // found_file = 1;
            // return found_file;
        }
        int size = octal_decimal(atoi(tar_p->size));
        int padding = 0;
        if (size % 512 != 0)
        {
            padding = 512 - size % 512;
        }
        temp = temp + 512 + size + padding;
        tar_p = (struct posix_header_ustar *) temp;
        //         print_posix_header(tar_p);
        // printk("\n temp : %x",temp);
    }
    return file_address;
}
uint64_t get_address_file(char * fileName, char *binary_tarfs_start,
        char* binary_tarfs_end)
{
    //printk("Indisde : %s",fileName);
    uint64_t file_address = 0;
    struct posix_header_ustar *tar_p =
            (struct posix_header_ustar *) (binary_tarfs_start);
    // print_posix_header(tar_p);
    //printk("\n Binary Start  %x", binary_tarfs_start);
    // printk("\n Binary End    %x", binary_tarfs_end);
    char *temp = binary_tarfs_start;
    // int size = octal_decimal(atoi(tar_p->size));
    while (temp < binary_tarfs_end)
    {
        if (matchString(tar_p->name, fileName) == 0)
        {
            int size = octal_decimal(atoi(tar_p->size));
            if (size <= 0)
            {
                return file_address;
            }
            file_address = (uint64_t) (temp + 512);
            return file_address;
            // found_file = 1;
            // return found_file;
        }
        int size = octal_decimal(atoi(tar_p->size));
        int padding = 0;
        if (size % 512 != 0)
        {
            padding = 512 - size % 512;
        }
        temp = temp + 512 + size + padding;
        tar_p = (struct posix_header_ustar *) temp;
        //     print_posix_header(tar_p);
        // printk("\n temp : %x",temp);
    }
    return file_address;
}
int count_characters(const char *str, char character)
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
int contains(const char *str1, const char *str2)
{
    while (*str1 && *str2)
    {
        if (*str1 != *str2)
        {
            return 0;
        }
        str1++;
        str2++;
    }
    if ((!*str2))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
char* get_name_file(uint64_t address)
{
    struct posix_header_ustar *tar_p = (struct posix_header_ustar *) address;
    return (char *) tar_p->name;
}
uint64_t get_next_pointer(uint64_t previous)
{
    struct posix_header_ustar *tar_p = (struct posix_header_ustar *) previous;
    char *temp = (char*) previous;
    // print_posix_header(tar_p);
    int size = octal_decimal(atoi(tar_p->size));
    int padding = 0;
    if (size % 512 != 0)
    {
        padding = 512 - size % 512;
    }
    temp = temp + 512 + size + padding;
    struct posix_header_ustar *next = (struct posix_header_ustar *) temp;
    //  char *next_f = (char *) next->name;
    // print_posix_header(next);
    return ((uint64_t) next);
}
/*
 Code Reference :  My Previous Submission
 */
int get_next_file(uint64_t previous, uint64_t base)
{
    struct posix_header_ustar *start = (struct posix_header_ustar *) base;
    //print_posix_header(start);
    char *start_f = (char *) start->name;
    char *temp_start_f = (char *) start->name;
    char *temp = (char*) previous;
    struct posix_header_ustar *tar_p = (struct posix_header_ustar *) previous;
    char *previous_f = (char *) tar_p->name;
    print_posix_header(tar_p);
    int size = octal_decimal(atoi(tar_p->size));
    int padding = 0;
    if (size % 512 != 0)
    {
        padding = 512 - size % 512;
    }
    temp = temp + 512 + size + padding;
    struct posix_header_ustar *next = (struct posix_header_ustar *) temp;
    char *next_f = (char *) next->name;
    if (contains(next_f, start_f))
    {
        while (*next_f == *start_f)
        {
            next_f++;
            start_f++;
        }
        //   printk("\n %s \n", next_f);
        char next_file_name[50] = { 0 };
        int i = 0;
        while (*next_f != '\0')
        {
            if (*next_f == '/')
            {
                break;
            }
            next_file_name[i] = *next_f;
            next_f++;
            i++;
        }
        next_file_name[i] = '\0';
        //   printk("%s",next_file_name);
        // printk("\n %s ", temp_start_f);
        if (matchString(previous_f, temp_start_f) == 0)
        {
            // printk("\n Matced");
            return 3;
        }
        while (*previous_f == *temp_start_f)
        {
            previous_f++;
            temp_start_f++;
        }
        //    printk("\n sss: %s ", previous_f);
        char previous_file_name[50] = { 0 };
        i = 0;
        while (*previous_f != '\0')
        {
            if (*previous_f == '/')
            {
                break;
            }
            previous_file_name[i] = *previous_f;
            previous_f++;
            i++;
        }
        previous_file_name[i] = '\0';
        //   printk(" %s",previous_file_name);
        if (matchString(previous_file_name, next_file_name) == 0)
        {
            //     printk("\n Matced");
            return 2;
        }
        else
        {
            //  printk("\n Didinot match");
            return 3;
        }
        //   return 1;
    }
    else
    {
        return 1;
    }
    return 0;
}
int get_dir_id()
{
    int i = 1;
    for (i = 1; i < MAX_PROCESS; i++)
    {
        if (dir_id[i] == 0)
        {
            dir_id[i] = 1;
            return i;
        }
    }
    return -1;
}
int get_file_id()
{
    int i = 1;
    for (i = 1; i < MAX_PROCESS; i++)
    {
        if (file_id[i] == 0)
        {
            file_id[i] = 1;
            return i;
        }
    }
    return -1;
}
void print_posix_header(struct posix_header_ustar *p)
{
    //   printk("\n Posix header :  %s :%s :%s ",p->name,p->size,p->typeflag);
}
/*
 Code Reference :  My Previous Submission
 */
int is_dir(char * fileName, char *binary_tarfs_start, char* binary_tarfs_end)
{
    //printk("Indisde : %s",fileName);
    int found_file = 0;
    struct posix_header_ustar *tar_p =
            (struct posix_header_ustar *) (binary_tarfs_start);
    // print_posix_header(tar_p);
    //printk("\n Binary Start  %x", binary_tarfs_start);
    // printk("\n Binary End    %x", binary_tarfs_end);
    char *temp = binary_tarfs_start;
    // int size = octal_decimal(atoi(tar_p->size));
    while (temp < binary_tarfs_end)
    {
        if (matchString(tar_p->name, fileName) == 0)
        {
            if (octal_decimal(atoi(tar_p->size)) == 0)
            {
                found_file = 1;
                return found_file;
            }
            found_file = 0;
            return found_file;
        }
        int size = octal_decimal(atoi(tar_p->size));
        int padding = 0;
        if (size % 512 != 0)
        {
            padding = 512 - size % 512;
        }
        temp = temp + 512 + size + padding;
        tar_p = (struct posix_header_ustar *) temp;
        //   print_posix_header(tar_p);
        // printk("\n temp : %x",temp);
    }
    return found_file;
}
int read_tarfs_file(char * fileName, char *binary_tarfs_start,
        char* binary_tarfs_end)
{
    //printk("Indisde : %s",fileName);
    int found_file = 0;
    struct posix_header_ustar *tar_p =
            (struct posix_header_ustar *) (binary_tarfs_start);
    // print_posix_header(tar_p);
    //printk("\n Binary Start  %x", binary_tarfs_start);
    // printk("\n Binary End    %x", binary_tarfs_end);
    char *temp = binary_tarfs_start;
    // int size = octal_decimal(atoi(tar_p->size));
    while (temp < binary_tarfs_end)
    {
        if (matchString(tar_p->name, fileName) == 0)
        {
            found_file = 1;
            return found_file;
        }
        int size = octal_decimal(atoi(tar_p->size));
        int padding = 0;
        if (size % 512 != 0)
        {
            padding = 512 - size % 512;
        }
        temp = temp + 512 + size + padding;
        tar_p = (struct posix_header_ustar *) temp;
        //   print_posix_header(tar_p);
        // printk("\n temp : %x",temp);
    }
    return found_file;
}
int octal_decimal(int n)
{
    int decimal = 0, i = 0, rem;
    while (n != 0)
    {
        rem = n % 10;
        n /= 10;
        int k = 0;
        int count = 8;
        int sum = 1;
        for (; k < i; k++)
        {
            sum = sum * count;
        }
        decimal += rem * sum;
        ++i;
    }
    return decimal;
}
int strlen(const char *str)
{
    const char *s;
    for (s = str; *s; ++s)
        ;
    return (s - str);
}
/*
 Code Reference :  My Previous Submission
 */
int get_elf_file(char *binary_tarfs_start, char * filename,
        char* binary_tarfs_end)
{
    int found_file = 0;
    struct posix_header_ustar *tar_p =
            (struct posix_header_ustar *) (binary_tarfs_start);
    // print_posix_header(tar_p);
    // printk("\n Binary Start  %x", binary_tarfs_start);
    //printk("\n Binary End    %x", binary_tarfs_end);
    char *temp = binary_tarfs_start;
    // int size = octal_decimal(atoi(tar_p->size));
    while (temp < binary_tarfs_end)
    {
        if (matchString(tar_p->name, filename) == 0)
        {
            found_file = 1;
            break;
            //    return found_file;
        }
        int size = octal_decimal(atoi(tar_p->size));
        int padding = 0;
        if (size % 512 != 0)
        {
            padding = 512 - size % 512;
        }
        temp = temp + 512 + size + padding;
        tar_p = (struct posix_header_ustar *) temp;
        print_posix_header(tar_p);
        // printk("\n temp : %x",temp);
    }
    if (found_file == 0)
    {
        return 0;
    }
    else
    {
        temp = temp + 512;
    }
    // printk("\n size : %d",atoi(tar_p->size) );
    //   printk("\n Header:");
    elf_start = (uint64_t) temp;
    //     printk("\n temp: %x",temp);
    parse_ELF(temp, atoi(tar_p->size), &exeFormat, &pdr);
    //printk("\n exe format :%d", Parse_ELF_Executable(temp, atoi(tar_p->size),&exeFormat,&pdr));
    return 1;
}
void switch0to3(uint64_t entryAddress)
{
    __asm volatile("pushq %%rax\n\t"
            "pushq %%rbx\n\t"
            "pushq %%rcx\n\t"
            "pushq %%rdx\n\t"
            "pushq %%rsi\n\t"
            "pushq %%rdi\n\t"
            "pushq %%r8\n\t"
            "pushq %%r9\n\t"
            "pushq %%r10\n\t"
            "pushq %%r11\n\t"
            : : :"memory");
    uint64_t rsp2;
    __asm__ __volatile__("movq %%rsp,%0;":"=g"(rsp2));
    printk("\n Rsp %x", rsp2);
    //  uint64_t rsp1;
    //  __asm volatile("movq (%%rsp), %0;":"=g"(tss.rsp0));
    tss.rsp0 = (uint64_t) rsp2;
    int a = 0x2B;
    //   int a = 0x28;
    __asm volatile("movq %0,%%rax;\n\t"
            "ltr (%%rax);"::"r"(&a));
    // printk("\n tss %x:",tss.rsp0);
    char * p = (char*) USER_STACK_START_VIRTUAL_ADDRESS;
    __asm volatile("pushq $0x23\n\t"
            "pushq %0\n\t"
            "pushfq \n\t"
            "popq %%rbx\n\t"
            "pushq %%rbx\n\t"
            "pushq $0x1b\n\t"
            "pushq %1\n\t"
            "iretq"
            : :"c"(p),"d"(entryAddress) :"memory");
}
/*
 Code Reference :  My Previous Submission
 */
uint64_t map_temp()
{
    uint64_t entryAddress = (uint64_t) exeFormat.entryAddr;
    //  mm_struct->entryAddress = entryAddress;
    int i = 0;
    for (; i < exeFormat.numSegments; i++)
    {
        struct Exe_Segment segment =
                (struct Exe_Segment) exeFormat.segmentList[i];
        uint64_t start = segment.vaddr;
        uint64_t end = segment.vaddr + segment.sizeInMemory;
        uint64_t pageNeeded = 0;
        uint64_t least_start = 0;
        uint64_t max_end = 0;
        if (end - start != 0)
        {
            least_start = (start / 0x1000) * 0x1000;
            max_end = (end / 0x1000) * 0x1000 + 0x1000;
            pageNeeded = (max_end - least_start) / 0x1000;
            // printk("\nPage Needed :%x",pageNeeded);
        }
        if (pageNeeded != 0)
        {
            while (pageNeeded != 0)
            {
                void *physicalAddress = page_alloc();
                //  printk("\n Physical Address :%x",physicalAddress);
                map_process(least_start, (uint64_t) physicalAddress);

                pageNeeded -= 1;
                least_start += 0x1000;
            }
            uint64_t ondiskstart = segment.offsetInFile + elf_start;
            //uint64_t ondiskfinish = segment.offsetInFile + elf_start + segment.sizeInMemory;
            //      printk("\n ondiskstart: %x odiskfinish: %x", ondiskstart,ondiskfinish);
            uint64_t size = segment.sizeInMemory;
            char *ondisk = (char *) ondiskstart;
            char *vadd = (char *) start;
            //      printk("\nvadd %x",vadd);
            while (size)
            {
                *vadd = *ondisk;
                vadd++;
                ondisk++;
                size--;
            }
            //      printk("\nvadd %x",vadd);
        }
    }
    return entryAddress;
}
/*
 Code Reference :  My Previous Submission
 */
void map_exe(struct mm_struct * mm_struct)
{
    //  printk("\n Map Exe");
    //  while(1);
    // printk("\nStart Address %x",exeFormat.entryAddr);
    uint64_t entryAddress = (uint64_t) exeFormat.entryAddr;
    mm_struct->entryAddress = entryAddress;
    int i = 0;
    for (; i < exeFormat.numSegments; i++)
    {
        struct Exe_Segment segment =
                (struct Exe_Segment) exeFormat.segmentList[i];
        uint64_t start = segment.vaddr;
        uint64_t end = segment.vaddr + segment.sizeInMemory;
        uint64_t pageNeeded = 0;
        uint64_t least_start = 0;
        uint64_t max_end = 0;
        if (end - start != 0)
        {
            least_start = (start / 0x1000) * 0x1000;
            max_end = (end / 0x1000) * 0x1000 + 0x1000;
            //   printk("\n least_start : %x",least_start);
            // printk("\n max_end : %x",max_end);
            pageNeeded = (max_end - least_start) / 0x1000;
            //printk("\nPage Needed :%x",pageNeeded);
        }
        if (pageNeeded != 0)
        {
            struct vm_area_struct* vm_area =
                    (struct vm_area_struct *) virtual_alloc();
            vm_area->vm_start = least_start;
            vm_area->vm_end = max_end;
            mm_struct->current->vm_next = vm_area;
            mm_struct->current = vm_area;
            mm_struct->current->vm_next = NULL;
            while (pageNeeded != 0)
            {
                void *physicalAddress = page_alloc();
                //  printk("\n Physical Address :%x",physicalAddress);
                map_process(least_start, (uint64_t) physicalAddress);

                pageNeeded -= 1;
                least_start += 0x1000;
            }
            uint64_t ondiskstart = segment.offsetInFile + elf_start;
            uint64_t size = segment.sizeInMemory;
            char *ondisk = (char *) ondiskstart;
            char *vadd = (char *) start;
            //        printk("\n Copying ... ");
            while (size)
            {
                *vadd = *ondisk;
                vadd++;
                ondisk++;
                size--;
            }
        }
    }
}
/*
 Code Reference :  My Previous Submission
 */
void map_exe_format()
{
    int i = 0;
    for (; i < exeFormat.numSegments; i++)
    {
        struct Exe_Segment segment =
                (struct Exe_Segment) exeFormat.segmentList[i];
        uint64_t start = segment.vaddr;
        uint64_t end = segment.vaddr + segment.sizeInMemory;
        // printk("\n Start: %x End: %x", start ,end);
        uint64_t pageNeeded = 0;
        uint64_t least_start = 0;
        uint64_t max_end = 0;
        if (end - start != 0)
        {
            least_start = (start / 0x1000) * 0x1000;
            max_end = (end / 0x1000) * 0x1000 + 0x1000;
            //   printk("\n least_start : %x",least_start);
            // printk("\n max_end : %x",max_end);
            pageNeeded = (max_end - least_start) / 0x1000;
            // printk("\nPage Needed :%x",pageNeeded);
        }
        if (pageNeeded != 0)
        {
            while (pageNeeded != 0)
            {
                void *physicalAddress = page_alloc();
                //  printk("\n Physical Address :%x",physicalAddress);
                map_process(least_start, (uint64_t) physicalAddress);
                pageNeeded -= 1;
                least_start += 0x1000;
            }
            uint64_t ondiskstart = segment.offsetInFile + elf_start;
            uint64_t size = segment.sizeInMemory;
            char *ondisk = (char *) ondiskstart;
            char *vadd = (char *) start;
            //	printk("\nvadd %x",vadd);
            while (size)
            {
                *vadd = *ondisk;
                vadd++;
                ondisk++;
                size--;
            }
            //	printk("\nvadd %x",vadd);
        }
    }
}
uint64_t get_entry_address()
{
    return exeFormat.entryAddr;
}
int matchString(char *s, char *t)
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
