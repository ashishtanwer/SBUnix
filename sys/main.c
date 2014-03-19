#include <defs.h>
#include <gdt.h>
#include <tarfs.h>
#include <pci.h>
#include <stdio.h>
#include <pmm.h>
#include <vmm.h>
#include <pic.h>
#include <pti.h>
#include <idt.h>
#include <scheduler.h>
#include <process.h>
extern struct PML4 *pml4;
uint64_t binary_start;
uint64_t binary_end;
void start(uint32_t* modulep, void* physbase, void* physfree)
{
    init_physbase_physfree((uint64_t) physbase, (uint64_t) physfree);
    // printk("\n\n Physbase %x : Physfree %x",get_physbase(),get_physfree());
    init_mmap();
    init_ahci();
    struct smap_t
    {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;
    while (modulep[0] != 0x9001)
        modulep += modulep[1] + 2;
    for (smap = (struct smap_t*) (modulep + 2);
            smap < (struct smap_t*) ((char*) modulep + modulep[1] + 2 * 4);
            ++smap)
    {
        if (smap->type == 1 /* memory */&& smap->length != 0)
        {
            init_region(smap->base, smap->base + smap->length);
        }
    }
    init_queue();
    init_tarfs();
    map_kernel();
    identity_mapping();
    load_cr3();
    pci_init();
    printk("pci_init complete\n");
    binary_start = (uint64_t) &_binary_tarfs_start;
    binary_end = (uint64_t) &_binary_tarfs_end;
    init_pcb();
    init_process_creation();
    char *filename = "bin/shell";
    add_process(filename, &_binary_tarfs_start, &_binary_tarfs_end);
    while (1)
        ;
}
#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;
void boot(void)
{
    // note: function changes rsp, local stack variables can't be practically used
    register char *temp1, *temp2;
    __asm__(
            "movq %%rsp, %0;"
            "movq %1, %%rsp;"
            :"=g"(loader_stack)
            :"r"(&stack[INITIAL_STACK_SIZE])
    );
    reload_gdt();
    setup_tss();
    starting_up_vga();
    init_pic(0x20, 0x28);
    populate_idt_table(0x8);
    init_timer();
    start(
            (uint32_t*) ((char*) (uint64_t) loader_stack[3]
                    + (uint64_t) &kernmem - (uint64_t) &physbase), &physbase,
            (void*) (uint64_t) loader_stack[4]);
    for (temp1 = "!!!!! start() returned !!!!!", temp2 = (char*) 0xb8000;
            *temp1; temp1 += 1, temp2 += 2)
        *temp2 = *temp1;
    while (1)
        ;
}
