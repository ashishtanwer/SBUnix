#include<stdio.h>
#include<defs.h>
#include<pmm.h>
#include<ahci.h>
#include<vmm.h>
#define PHYS_MEM_GB 64
#define MEM_MAP_SIZE 256
#define PHYS_PAGE_SIZE 4096
#define PHYS_BLOCK_SIZE 512
#define MEM_NOTAVAILABLE 0
#define AHCI_SIZE 122880
#define IDENTITY_MAP_VIRTUAL_START 0xFFFFFFFFF0000000UL
// free list will contain 256 Entries, Where every entry is a 64 bit uint  so every entry represents 64*4096 pages
static uint64_t ker_physbase;
static uint64_t ker_physfree;
uint64_t *mem_map;
static uint64_t max_phys;
void init_physbase_physfree(uint64_t physbase, uint64_t physfree)
{
    ker_physbase = physbase;
    ker_physfree = physfree;
}
void set_physfree(uint64_t physfree)
{
    ker_physfree = physfree;
}
void set_physbase(uint64_t physbase)
{
    ker_physbase = physbase;
}
uint64_t get_physfree()
{
    return ker_physfree;
}
void set_max_phys(uint64_t max)
{
    max_phys = max;
}
uint64_t get_max_phys()
{
    return max_phys;
}
uint64_t get_physbase()
{
    return ker_physbase;
}
void mmap_set(uint64_t bit)
{
    mem_map[bit / 64] |= (1ull << (bit % 64));
}
void mmap_unset(uint64_t bit)
{
    mem_map[bit / 64] &= ~(1ull << (bit % 64));
}
int mmap_test(int bit)
{
    return mem_map[bit / 64] & (1ull << (bit % 64));
}
void init_mmap()
{
    uint64_t physfree = get_physfree();
    mem_map = (uint64_t*) ker_physfree;
    uint64_t i = 0;
    for (i = 0; i < MEM_MAP_SIZE; i++)
    {
        mem_map[i] = MEM_NOTAVAILABLE;
    }
    physfree += (uint64_t) (MEM_MAP_SIZE * 64 / 8);
    set_physfree(physfree);
}
void init_ahci()
{
    uint64_t physfree = get_physfree();
    //uint64_t *local_pages_for_ahci_start=(uint64_t *)physfree;
    set_AHCI_Paddress(physfree);
    //printk("\n Physfree before increment %x",get_physfree());
    physfree += (uint64_t) (AHCI_SIZE);
    set_physfree(physfree);
    //printk("\n Physfree after increment %x",get_physfree());
}
void init_region(uint64_t base, uint64_t limit)
{
    set_max_phys(limit);
    int start = base / PHYS_PAGE_SIZE;
    int end = limit / PHYS_PAGE_SIZE;
    mmap_unset(0);
    mmap_unset(1);
    for (; start <= end; start++)
    {
        if ((start * PHYS_PAGE_SIZE) <= ker_physfree
                && (start * PHYS_PAGE_SIZE) >= ker_physbase)
        {
        }
        else
        {
            mmap_set(start);
        }
    }
    int sta = 0;
    for (uint64_t low = 0; low < 0x100000; low += 0x1000)
    {
        sta++;
        mmap_unset(sta);
    }
}
uint64_t get_index_of_free_page()
{
    uint64_t i = 0;
    for (i = 0; i < MEM_MAP_SIZE; i++)
    {
        uint64_t k = 0;
        for (k = 0; k < 64; k++)
        {
            uint64_t bit = 1ull << k;
            if (mem_map[i] & bit)
            {
                //                        printk("\nFree Memory at bit %d: of block %d",k,i);
                //       printk("   %d",i*64 + k);
                return i * 64 + k;
            }
        }
    }
    return -1;
}
void* page_alloc()
{
    int index = get_index_of_free_page();
    if (index == -1)
        return NULL; //out of memory
    mmap_unset(index);
    uint64_t addr = index * PHYS_PAGE_SIZE;
    return (void*) addr;
}
void page_dealloc(void* p)
{
    uint64_t addr = (uint64_t) p;
    int index = addr / PHYS_PAGE_SIZE;
    mmap_set(index);
}
