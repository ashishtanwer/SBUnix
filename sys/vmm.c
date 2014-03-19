#include<stdio.h>
#include<defs.h>
#include<vmm.h>
#include<pmm.h>
#include<process.h>
#define ADDITIONAL 0x4800
// #define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
// #define IDENTITY_MAP_PHYSICAL_START 0x0UL
// #define KERNEL_STACK_VIRTUAL_ADDRESS 0xFFFFFFFFFFF00000UL
// #define USER_STACK_VIRTUAL_ADDRESS 0xFFFFFFFF00000000UL
// #define HEAP_VIRTUAL_ADDRESS       0xFFFFFFF000000000UL
void add_attribute(uint64_t* entry, uint64_t attribute)
{
    *entry |= attribute;
}
void delete_attribute(uint64_t* entry, uint64_t attribute)
{
    *entry &= ~attribute;
}
void set_address(uint64_t* entry, uint64_t address)
{
    *entry = (*entry & ~FRAME) | address;
}
uint64_t get_address(uint64_t* entry)
{
    return (*entry & FRAME);
}
int is_present(uint64_t* entry)
{
    return (*entry & PRESENT);
}
int is_writable(uint64_t* entry)
{
    return (*entry & WRITABLE);
}
int is_user(uint64_t* entry)
{
    return (*entry & USER_MODE);
}
uint64_t* pml4_lookup(struct PML4 *pml4, uint64_t virtual_address)
{
    return &pml4->entries[PML4_INDEX(virtual_address)];
}
uint64_t* pdpt_lookup(struct PDPT *pdpt, uint64_t virtual_address)
{
    return &pdpt->entries[PDPT_INDEX(virtual_address)];
}
uint64_t* pdt_lookup(struct PDT *pdt, uint64_t virtual_address)
{
    return &pdt->entries[PDT_INDEX(virtual_address)];
}
uint64_t* pt_lookup(struct PT *pt, uint64_t virtual_address)
{
    return &pt->entries[PT_INDEX(virtual_address)];
}
#define VGA_BUFFER 0xB8000 
#define AHCI_MMIO_BUFFER 0xfebf0000
#define e1000_MMIO_BUFFER 0xfebc0000
extern char kernmem;
struct PML4 *pml4;
extern char *starting_address_of_VGA_buffer;
extern uint64_t *mem_map;
uint64_t virtual_address_vga;
uint64_t virtual_address_ahci_mmio;
uint64_t virtual_address_e1000_mmio;
uint64_t physical_address_ahci;
uint64_t virtual_address_k;
uint64_t virtual_address_pmlf4;
uint64_t cr3;
//#define UFLAGS 1|2|4
//#define KFLAGS 1|2
//#define RW 0x2
//#define PR 0x1        //Present
//#define US 0x4       //User -1/Supervisor -0
uint64_t COW = 0x1000000000000;
uint64_t READ = ~WRITABLE;
extern struct queue_descriptor * descriptor;
extern struct queue_descriptor * wait_queue;
extern struct queue_descriptor * zombie_queue;
uint64_t virtualBaseAddr = IDENTITY_MAP_VIRTUAL_START;
uint64_t virtual_address_kernmem;
uint64_t* get_AHCI_Paddress()
{
    return physical_address_ahci;
}
void set_AHCI_Paddress(uint64_t ahci_address)
{
    physical_address_ahci = ahci_address;
}
uint64_t get_VGA_Vaddress()
{
    return virtual_address_vga;
}
void set_VGA_Vaddress(uint64_t vga_address)
{
    virtual_address_vga = vga_address;
}
uint64_t get_AHCI_MMIO_Vaddress()
{
    return virtual_address_ahci_mmio;
}
void set_AHCI_MMIO_Vaddress(uint64_t mmio_address)
{
    virtual_address_ahci_mmio = mmio_address;
}
uint64_t get_e1000_MMIO_Vaddress()
{
    return virtual_address_e1000_mmio;
}
void set_e1000_MMIO_Vaddress(uint64_t mmio_address)
{
    virtual_address_e1000_mmio = mmio_address;
}
uint64_t get_PMLF4_Vaddress()
{
    return virtual_address_pmlf4;
}
void set_PMLF4_Vaddress(uint64_t pmlf4_address)
{
    virtual_address_pmlf4 = pmlf4_address;
}
//extern static uint64_t *mem_map; 
/*
 Code Reference :  My Previous Submission
 */
void map_kernel()
{
    uint64_t physbase = get_physbase();
    uint64_t physfree = get_physfree();
    pml4 = (struct PML4 *) page_alloc();
    //  printk("\n pmlf4 :%x",pml4->entries[23]);
    struct PDPT *pdpt = (struct PDPT *) page_alloc();
    uint64_t pdpt_entry = (uint64_t) pdpt;
    //  printk("\nentry %x",pdpt_entry);
    add_attribute(&pdpt_entry, PRESENT);
    add_attribute(&pdpt_entry, WRITABLE);
    add_attribute(&pdpt_entry, USER_MODE);
    // printk("\nkernmem:%x",&kernmem);
    //  printk("\nentry :%x",pdpt_entry);
    pml4->entries[PML4_INDEX((uint64_t )&kernmem)] = pdpt_entry;
    struct PDT *pdt = (struct PDT *) page_alloc();
    uint64_t pdt_entry = (uint64_t) pdt;
    add_attribute(&pdt_entry, PRESENT);
    add_attribute(&pdt_entry, WRITABLE);
    add_attribute(&pdt_entry, USER_MODE);
    pdpt->entries[PDPT_INDEX((uint64_t )&kernmem)] = pdt_entry;
    struct PT *pt = (struct PT *) page_alloc();
    uint64_t pt_entry = (uint64_t) pt;
    add_attribute(&pt_entry, PRESENT);
    add_attribute(&pt_entry, WRITABLE);
    add_attribute(&pt_entry, USER_MODE);
    pdt->entries[PDT_INDEX((uint64_t )&kernmem)] = pt_entry;
    //  printk("\n PLM index:%d",PML4_INDEX((uint64_t)&kernmem));
    // printk("\n PDPT index:%d",PDPT_INDEX((uint64_t)&kernmem));
    // printk("\n PDT index:%d",PDT_INDEX((uint64_t)&kernmem));
    // printk("\n PT index:%d",PDT_INDEX((uint64_t)&kernmem));
    uint64_t virtualAddr = (uint64_t) &kernmem;
    virtual_address_kernmem = virtualAddr;
    // uint64_t additional = 0x4800;
    physfree = ADDITIONAL + physfree;
    //    printk("\n Physfree: %x",physfree);
    while (physbase < physfree)
    {
        uint64_t entry = physbase;
        add_attribute(&entry, PRESENT);
        add_attribute(&entry, WRITABLE);
        add_attribute(&entry, USER_MODE);
        pt->entries[PT_INDEX(virtualAddr)] = entry;
        //printk("\nentry of kernel vitual address  %x at index %d is %x ",virtualAddr,PT_INDEX((uint64_t)virtualAddr),pt->entries[PT_INDEX((uint64_t)virtualAddr)] );
        physbase = physbase + 0x1000;
        virtualAddr += 0x1000;
    }
    // VGA BUFFER
    set_VGA_Vaddress(virtualAddr);
    virtualAddr += 0x5000;
    set_AHCI_MMIO_Vaddress(virtualAddr);
    virtualAddr += 0x1000;
    set_e1000_MMIO_Vaddress(virtualAddr);
    virtualAddr += 0x20000;
    virtual_address_k = virtualAddr;
}
void setup_kernel_stack()
{
    // struct PML4 *new_pml4 = (struct PML4 *)getCr3();
    // pml4 = new_pml4;
    struct kernel_stack *stack = (struct kernel_stack *) page_alloc();
    //     printk("\nkernel stack address physical:%x", stack);
    map_process(KERNEL_STACK_VIRTUAL_ADDRESS, (uint64_t) stack);
    // map_process(KERNEL_STACK_VIRTUAL_ADDRESS, (uint64_t)stack);
    // printk("\nKernel Stack virtual address %x \n  mapped to physical address %x", KERNEL_STACK_VIRTUAL_ADDRESS, stack);
}
//uint64_t add = 0x1000;
void setup_init_kernel_stack()
{
    struct kernel_stack *stack = (struct kernel_stack *) page_alloc();
    map_page(KERNEL_STACK_VIRTUAL_ADDRESS, (uint64_t) stack);
}
void setup_user_stack()
{
    struct PML4 *newpml4 = (struct PML4 *) get_Cr3();
    pml4 = newpml4;
    struct user_stack *stack = (struct user_stack *) page_alloc();
    map_process(USER_STACK_VIRTUAL_ADDRESS, (uint64_t) stack);
}
void setup_heap()
{
    void *heap = page_alloc();
    map_process(HEAP_VIRTUAL_ADDRESS, (uint64_t) heap);
}
/*
 Code Reference :  My Previous Submission
 */
void map_process(uint64_t virtual_address, uint64_t physical_address)
{
    struct PDPT *pdpt;
    struct PDT *pdt;
    struct PT *pt;
    pml4 = (struct PML4*) get_Cr3();
    pml4 = (struct PML4*) (IDENTITY_MAP_VIRTUAL_START | (uint64_t) pml4);
    uint64_t pml4_entry = pml4->entries[PML4_INDEX((uint64_t )virtual_address)];
    //     printk("\n pmlf_entry :%x",pml4_entry);
    if (is_present(&pml4_entry))
    {
        pdpt = (struct PDPT *) get_address(&pml4_entry);
    }
    else
    {
        //    printk("\nPDPT not Present");
        pdpt = (struct PDPT *) page_alloc();
        uint64_t pdpt_entry = (uint64_t) pdpt;
        // printk("\nentry %x",pdpt_entry);
        add_attribute(&pdpt_entry, PRESENT);
        add_attribute(&pdpt_entry, WRITABLE);
        add_attribute(&pdpt_entry, USER_MODE_ON);
        // printk("\nentry %x",pdpt_entry);
        pml4->entries[PML4_INDEX((uint64_t )virtual_address)] = pdpt_entry;
    }
    uint64_t virtual_pdpt = (uint64_t) pdpt | IDENTITY_MAP_VIRTUAL_START;
    pdpt = (struct PDPT*) virtual_pdpt;
    //  printk("\n pdpt  virtual Address :%x ", virtual_pdpt);
    uint64_t pdpt_entry = pdpt->entries[PDPT_INDEX((uint64_t )virtual_address)];
    if (is_present(&pdpt_entry))
    {
        pdt = (struct PDT *) get_address(&pdpt_entry);
    }
    else
    {
        // printk("\n PDT not present");
        pdt = (struct PDT *) page_alloc();
        uint64_t pdt_entry = (uint64_t) pdt;
        add_attribute(&pdt_entry, PRESENT);
        add_attribute(&pdt_entry, WRITABLE);
        add_attribute(&pdt_entry, USER_MODE_ON);
        pdpt->entries[PDPT_INDEX((uint64_t )virtual_address)] = pdt_entry;
    }
    uint64_t virtual_pdt = (uint64_t) pdt | IDENTITY_MAP_VIRTUAL_START;
    pdt = (struct PDT*) virtual_pdt;
    // printk("\n pdt  virtual Address :%x ", virtual_pdt);
    //    printk("\nBase Address of pdt: %x",pdt);
    uint64_t pdt_entry = pdt->entries[PDT_INDEX((uint64_t )virtual_address)];
    //printk("\n Index %x",PDT_INDEX((uint64_t)virtual_address));
    // printk("\n pdt_entry %x",pdt_entry);
    if (is_present(&pdt_entry))
    {
        pt = (struct PT *) get_address(&pdt_entry);
    }
    else
    {
        // printk("\nPT not present");
        pt = (struct PT *) page_alloc();
        uint64_t pt_entry = (uint64_t) pt;
        add_attribute(&pt_entry, PRESENT);
        add_attribute(&pt_entry, WRITABLE);
        add_attribute(&pt_entry, USER_MODE_ON);
        pdt->entries[PDT_INDEX((uint64_t )virtual_address)] = pt_entry;
    }
    uint64_t virtual_pt = (uint64_t) pt | IDENTITY_MAP_VIRTUAL_START;
    pt = (struct PT*) virtual_pt;
    //printk("\n pt  virtual Address :%x ", virtual_pt);
    // printk("\nBase Address of pdt: %x",pt);
    // Just Mapping and not allocating
    uint64_t entry = physical_address;
    add_attribute(&entry, PRESENT);
    add_attribute(&entry, WRITABLE);
    add_attribute(&entry, USER_MODE_ON);
    //   printk("\n entry : %x ",entry);
    // pt->entries[PT_INDEX(virtualAddr)] = entry;
    pt->entries[PT_INDEX((uint64_t )virtual_address)] = entry;
}
/*
 Code Reference :  My Previous Submission
 */
uint64_t get_physical_address(uint64_t virtual_address)
{
    struct PML4 *pml4 = (struct PML4 *) get_Cr3();
    uint64_t physical_address = 0;
    struct PDPT *pdpt = NULL;
    struct PDT *pdt = NULL;
    struct PT *pt = NULL;
    //IDENTITY_MAP_VIRTUAL_START
    uint64_t pml4_entry = ((struct PML4 *) ((uint64_t) pml4
            | IDENTITY_MAP_VIRTUAL_START))->entries[PML4_INDEX(
            (uint64_t )virtual_address)];
    //   printk("\n PML4 Entry: %x",pml4_entry);
    if (is_present(&pml4_entry))
    {
        // printk("\n pdpt is present");
        pdpt = (struct PDPT *) (pml4_entry & 0xFFFFFFFFFFFFF000);
    }
    pdpt = ((struct PDPT *) ((uint64_t) pdpt | IDENTITY_MAP_VIRTUAL_START));
    uint64_t pdpt_entry = pdpt->entries[PDPT_INDEX((uint64_t )virtual_address)];
    // printk("\n pdpt Entry: %x",pdpt_entry);
    if (is_present(&pdpt_entry))
    {
        // printk("\n pdt is present");
        pdt = (struct PDT *) (pdpt_entry & 0xFFFFFFFFFFFFF000);
    }
    pdt = ((struct PDT *) ((uint64_t) pdt | IDENTITY_MAP_VIRTUAL_START));
    uint64_t pdt_entry = pdt->entries[PDT_INDEX((uint64_t )virtual_address)];
    // printk("\n pdt Entry: %x",pdt_entry);
    if (is_present(&pdt_entry))
    {
        // printk("\n pt is present");
        pt = (struct PT *) (pdt_entry & 0xFFFFFFFFFFFFF000);
    }
    if (pt != NULL)
    {
        //     printk("\n pt is null");
        pt = ((struct PT *) ((uint64_t) pt | IDENTITY_MAP_VIRTUAL_START));
        physical_address = pt->entries[PT_INDEX((uint64_t )virtual_address)];
    }
    printk("\n Physicall Address : %x", physical_address);
    return physical_address;
}
void map_page(uint64_t virtual_address, uint64_t physical_address)
{
    struct PDPT *pdpt;
    struct PDT *pdt;
    struct PT *pt;
    uint64_t pml4_entry = pml4->entries[PML4_INDEX((uint64_t )virtual_address)];
    //    printk("\n pmlf_entry :%x",pml4_entry);
    if (is_present(&pml4_entry))
    {
        pdpt = (struct PDPT *) get_address(&pml4_entry);
    }
    else
    {
        //printk("\nPDPT not Present");
        pdpt = (struct PDPT *) page_alloc();
        uint64_t pdpt_entry = (uint64_t) pdpt;
        //printk("\nentry %x",pdpt_entry);
        add_attribute(&pdpt_entry, PRESENT);
        add_attribute(&pdpt_entry, WRITABLE);
        add_attribute(&pdpt_entry, USER_MODE);
        pml4->entries[PML4_INDEX((uint64_t )virtual_address)] = pdpt_entry;
    }
    // printk("\nBase Address of pdpt: %x",pdpt);
    //printk("\nEntruuuuu:  %d",PDPT_INDEX((uint64_t)virtual_address));
    uint64_t pdpt_entry = pdpt->entries[PDPT_INDEX((uint64_t )virtual_address)];
    if (is_present(&pdpt_entry))
    {
        pdt = (struct PDT *) get_address(&pdpt_entry);
    }
    else
    {
        // printk("\n PDT not present");
        pdt = (struct PDT *) page_alloc();
        uint64_t pdt_entry = (uint64_t) pdt;
        add_attribute(&pdt_entry, PRESENT);
        add_attribute(&pdt_entry, WRITABLE);
        add_attribute(&pdt_entry, USER_MODE);
        pdpt->entries[PDPT_INDEX((uint64_t )virtual_address)] = pdt_entry;
    }
    //    printk("\nBase Address of pdt: %x",pdt);
    uint64_t pdt_entry = pdt->entries[PDT_INDEX((uint64_t )virtual_address)];
    //printk("\n Index %x",PDT_INDEX((uint64_t)virtual_address));
    // printk("\n pdt_entry %x",pdt_entry);
    if (is_present(&pdt_entry))
    {
        pt = (struct PT *) get_address(&pdt_entry);
    }
    else
    {
        // printk("\nPT not present");
        pt = (struct PT *) page_alloc();
        uint64_t pt_entry = (uint64_t) pt;
        add_attribute(&pt_entry, PRESENT);
        add_attribute(&pt_entry, WRITABLE);
        add_attribute(&pt_entry, USER_MODE);
        pdt->entries[PDT_INDEX((uint64_t )virtual_address)] = pt_entry;
    }
    // printk("\nBase Address of pdt: %x",pt);
    // Just Mapping and not allocating
    uint64_t entry = physical_address;
    add_attribute(&entry, PRESENT);
    add_attribute(&entry, WRITABLE);
    add_attribute(&entry, USER_MODE);
    pt->entries[PT_INDEX((uint64_t )virtual_address)] = entry;
}
/*
 Code Reference :  My Previous Submission
 */
void identity_mapping()
{
    uint64_t virtualaddr = IDENTITY_MAP_VIRTUAL_START;
    uint64_t physaddr = IDENTITY_MAP_PHYSICAL_START;
    uint64_t max_physical = get_max_phys();
    while (physaddr <= max_physical)
    {
        map_page(virtualaddr, physaddr);
        physaddr = physaddr + 0x1000UL;
        virtualaddr = virtualaddr + 0x1000UL;
        // }
        //  a++;
    }
    map_page(get_VGA_Vaddress(), VGA_BUFFER);
    map_page(get_AHCI_MMIO_Vaddress(), AHCI_MMIO_BUFFER);
    uint64_t e1000_virtualaddr_ptr = get_e1000_MMIO_Vaddress();
    uint64_t e1000_MMIO_BUFFER_ptr = e1000_MMIO_BUFFER;
    while (e1000_virtualaddr_ptr <= virtual_address_k)
    {
        map_page(e1000_virtualaddr_ptr, e1000_MMIO_BUFFER_ptr);
        e1000_virtualaddr_ptr = e1000_virtualaddr_ptr + 0x1000UL;
        e1000_MMIO_BUFFER_ptr = e1000_MMIO_BUFFER_ptr + 0x1000UL;
        // }
        //  a++;
    }
    starting_address_of_VGA_buffer = (char *) get_VGA_Vaddress();
    starting_up_vga();
    mem_map = (uint64_t*) (0xFFFFFFFF80000000UL | (uint64_t) mem_map);
    descriptor = (struct queue_descriptor*) (0xFFFFFFFF80000000UL
            | (uint64_t) descriptor);
    wait_queue = (struct queue_descriptor*) (0xFFFFFFFF80000000UL
            | (uint64_t) wait_queue);
    zombie_queue = (struct queue_descriptor*) (0xFFFFFFFF80000000UL
            | (uint64_t) zombie_queue);
    uint64_t v_add = (uint64_t) pml4 | IDENTITY_MAP_VIRTUAL_START;
    set_PMLF4_Vaddress(v_add);
}
void* allocation_and_map(uint64_t virtual_address)
{
    struct PML4 *new_pml4 = (struct PML4 *) get_Cr3();
    pml4 = new_pml4;
    uint64_t physical_address = (uint64_t) page_alloc();
    map_process(virtual_address, physical_address);
    virtual_address += 0x1000;
    return (void*) (virtual_address);
}
void load_cr3()
{
    uint64_t pml4addr = (uint64_t) pml4;
    // set_CR3(pml4);
    __asm__ __volatile__("mov %0, %%cr3":: "b"(pml4addr));
    /*
     __asm__ __volatile__( "inb  %1, %0;"
     :"=a" (ret)
     :"d"  (port)   );
     */
}
/*
 Code Reference :  Adopted from code Reference from Sumanth
 */
void* AssignPhysicalToVirtual(uint64_t virtAddr, int pages)
{
    int cnt = 0;
    uint64_t temp = virtAddr;
    struct PML4 *newpml4 = (struct PML4 *) get_Cr3();
    pml4 = newpml4;
    for (cnt = 0; cnt < pages; cnt++)
    {
        uint64_t phyPage = (uint64_t) page_alloc();
        map_process(virtAddr, phyPage);
        //   map_page((void*)phyPage,(void *)temp,7);
        temp += 0x1000;
    }
    return (void*) (virtAddr);
}
void SetCr3(struct PML4 * current)
{
    uint64_t pml4addr = (uint64_t) current;
    __asm volatile("mov %0, %%cr3":: "b"(pml4addr));
}
uint64_t get_Cr3()
{
    uint64_t cr3Value;
    __asm volatile("mov %%cr3, %0" : "=r" (cr3Value));
    return cr3Value;
}
void memset(void *init, char val, int sz)
{
    int i = 0;
    for (; i < sz; i++)
        ((char*) init)[i] = 0;
}
/*
 Code Reference :  Adopted from code Reference from Sumanth
 */
uint64_t lastUsedAddress = IDENTITY_MAP_VIRTUAL_START;
void* virtual_alloc()
{
    uint64_t phyPage = (uint64_t) page_alloc();
    //  printk("\n PhyPage %x",phyPage);
    lastUsedAddress += 0x1000;
    struct PML4 *newpml4 = (struct PML4 *) get_Cr3();
    pml4 = newpml4;
    map_process(lastUsedAddress - 0x1000, phyPage);
    memset((void *) lastUsedAddress - 0x1000, 0, 4096);
    //  map_process(phyPage,lastUsedAddress-0x1000);
    return (void*) (lastUsedAddress - 0x1000);
}
void* user_memory_setup()
{
    struct PML4 *pml4New = (struct PML4 *) page_alloc();
    struct PML4 *pml4Current = (struct PML4 *) get_Cr3();
    ((struct PML4 *) ((uint64_t) pml4New | IDENTITY_MAP_VIRTUAL_START))->entries[511] =
            ((struct PML4 *) ((uint64_t) pml4Current
                    | IDENTITY_MAP_VIRTUAL_START))->entries[511];
    return (void*) pml4New;
}
void memcpy(void *from, void *to, int size)
{
    char* phyStart = (char*) from;
    char* addr = (char*) to;
    while (size--)
        *addr++ = *phyStart++;
}
void * fork_paging()
{
    struct PML4 *pml4_new = (struct PML4 *) page_alloc();
    struct PML4 *pml4_current = (struct PML4 *) get_Cr3();
    int i = 0;
    for (; i < 512; i++)
    {
        if (i >= PML4_INDEX(virtual_address_kernmem))
        {
            ((struct PML4 *) ((uint64_t) pml4_new | IDENTITY_MAP_VIRTUAL_START))->entries[i] =
                    ((struct PML4 *) ((uint64_t) pml4_current
                            | IDENTITY_MAP_VIRTUAL_START))->entries[i];
            continue;
        }
        uint64_t pmlf_current_entry = ((struct PML4 *) ((uint64_t) pml4_current
                | IDENTITY_MAP_VIRTUAL_START))->entries[i];
        if (is_present(&pmlf_current_entry))
        {
            //      printk("\n %d : Pmlf current entry : %x",i ,pmlf_current_entry);
            struct PDPT *pdpt_new = (struct PDPT *) page_alloc();
            uint64_t pdpt_entry_new = (uint64_t) pdpt_new;
            add_attribute(&pdpt_entry_new, PRESENT);
            add_attribute(&pdpt_entry_new, WRITABLE);
            add_attribute(&pdpt_entry_new, USER_MODE);
            ((struct PML4 *) ((uint64_t) pml4_new | IDENTITY_MAP_VIRTUAL_START))->entries[i] =
                    (uint64_t) pdpt_entry_new;
            struct PDPT *pdpt_current = (struct PDPT *) get_address(
                    &pmlf_current_entry);
            int j = 0;
            for (; j < 512; j++)
            {
                uint64_t pdpt_current_entry =
                        ((struct PDPT *) ((uint64_t) pdpt_current
                                | IDENTITY_MAP_VIRTUAL_START))->entries[j];
                if (is_present(&pdpt_current_entry))
                {
                    //printk("\n %d :  Pdpt current entry : %x",j ,pdpt_current_entry);
                    struct PDT *pdt_new = (struct PDT *) page_alloc();
                    uint64_t pdt_entry_new = (uint64_t) pdt_new;
                    add_attribute(&pdt_entry_new, PRESENT);
                    add_attribute(&pdt_entry_new, WRITABLE);
                    add_attribute(&pdt_entry_new, USER_MODE);
                    ((struct PDPT *) ((uint64_t) pdpt_new
                            | IDENTITY_MAP_VIRTUAL_START))->entries[j] =
                            (uint64_t) pdt_entry_new;
                    struct PDT *pdt_current = (struct PDT *) get_address(
                            &pdpt_current_entry);
                    int k = 0;
                    for (; k < 512; k++)
                    {
                        //  printk("\n %d :  Pdt current entry : %x",j ,pdpt_current_entry);
                        uint64_t pdt_current_entry =
                                ((struct PDT *) ((uint64_t) pdt_current
                                        | IDENTITY_MAP_VIRTUAL_START))->entries[k];
                        if (is_present(&pdt_current_entry))
                        {
                            //   printk("\n %d :  Pdt current entry : %x",k ,pdt_current_entry);
                            struct PT* pt_new = (struct PT*) page_alloc();
                            uint64_t pt_entry_new = (uint64_t) pt_new;
                            add_attribute(&pt_entry_new, PRESENT);
                            add_attribute(&pt_entry_new, WRITABLE);
                            add_attribute(&pt_entry_new, USER_MODE);
                            ((struct PDPT *) ((uint64_t) pdt_new
                                    | IDENTITY_MAP_VIRTUAL_START))->entries[k] =
                                    (uint64_t) pt_entry_new;
                            struct PT *pt_current = (struct PT *) get_address(
                                    &pdt_current_entry);
                            int l = 0;
                            for (; l < 512; l++)
                            {
                                // printk("\n %d :  Pdpt current entry : %x",j ,pdpt_current_entry);
                                uint64_t page_entry =
                                        ((struct PT *) ((uint64_t) pt_current
                                                | IDENTITY_MAP_VIRTUAL_START))->entries[l];
                                if (is_present(&page_entry))
                                {
                                    uint64_t entry = (uint64_t) page_alloc();
                                    uint64_t pag = (uint64_t) get_address(
                                            &page_entry);
                                    //printk("\n virtual entry %x",(IDENTITY_MAP_VIRTUAL_START|pag));
                                    memcpy(
                                            (void *) (IDENTITY_MAP_VIRTUAL_START
                                                    | (pag)),
                                            (void *) (IDENTITY_MAP_VIRTUAL_START
                                                    | entry), 4096);
                                    add_attribute(&entry, PRESENT);
                                    add_attribute(&entry, WRITABLE);
                                    add_attribute(&entry, USER_MODE);
                                    ((struct PT *) ((uint64_t) pt_new
                                            | IDENTITY_MAP_VIRTUAL_START))->entries[l] =
                                            entry;
                                    ((struct PT *) ((uint64_t) pt_current
                                            | IDENTITY_MAP_VIRTUAL_START))->entries[l] =
                                            page_entry;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return (void*) pml4_new;
}
