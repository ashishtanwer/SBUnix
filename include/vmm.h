#ifndef _VMM_H
#define _VMM_H
#include<defs.h>
#define PRESENT 0x1        
#define WRITABLE 0x2       
#define USER_MODE 0x4       
#define USER_MODE_ON 0x4
#define PWT 0x8      
#define PCD 0x10     
#define ACCESED 0x20      
#define DIRTY 0x40      
#define PAT 0x80     
#define GLOBAL 0x100      
#define NX 0x8000000000000 
#define FRAME 0xFFFFFFFFFFFFF000
#define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
#define IDENTITY_MAP_PHYSICAL_START 0x0UL
#define KERNEL_STACK_START_VIRTUAL_ADDRESS 0xFFFFFFFFFFF00aFFUL
#define KERNEL_STACK_VIRTUAL_ADDRESS 0xFFFFFFFFFFF00000UL
#define USER_STACK_START_VIRTUAL_ADDRESS 0xFFFFFFFF00000aFFUL
#define USER_STACK_VIRTUAL_ADDRESS 0xFFFFFFFF00000000UL
#define HEAP_START_VIRTUAL_ADDRESS 0xFFFFFFF000000FFFUL
#define HEAP_VIRTUAL_ADDRESS       0xFFFFFFF000000000UL
//uint64_t COW =  0x10000000000000;
void add_attribute(uint64_t*, uint64_t);
void delete_attribute(uint64_t*, uint64_t);
void set_address(uint64_t*, uint64_t);
uint64_t get_address(uint64_t*);
int is_present(uint64_t*);
int is_writable(uint64_t*);
int is_user(uint64_t*);
void map_page(uint64_t, uint64_t);
void map_kernel();
void load_cr3();
uint64_t get_VGA_Vaddress();
void set_VGA_Vaddress(uint64_t);
void setup_user_stack();
void setup_kernel_stack();
void setup_init_kernel_stack();
void setup_heap();
void map_process(uint64_t, uint64_t);
//uint64_t get_CR3();
//void set_CR3(uint64_t);
uint64_t get_physical_address(uint64_t);
//void SetCr3(struct PML4 * );
uint64_t get_Cr3();
void* virtual_alloc();
struct PML4
{
    uint64_t entries[512];
};
struct PDPT
{
    uint64_t entries[512];
};
struct PDT
{
    uint64_t entries[512];
};
struct PT
{
    uint64_t entries[512];
};
void SetCr3(struct PML4 *);
uint64_t get_Cr3();
void* user_memory_setup();
void * fork_paging();
void * AssignPhysicalToVirtual(uint64_t, int);
void* allocation_and_map(uint64_t);
#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PDT_INDEX(x) (((x) >> 21) & 0x1FF)
#define PT_INDEX(x) (((x) >> 12) & 0x1FF)
void identity_mapping();
#endif
