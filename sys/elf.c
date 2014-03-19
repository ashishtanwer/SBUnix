#include <elf.h>
#include <defs.h>
#include <stdio.h>
/*From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 * and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int elfDebug = 0;
int strncmp(const char *s1, const char *s2, uint32_t n)
{
    for (; n > 0; s1++, s2++, --n)
        if (*s1 != *s2)
            return ((*(unsigned char *) s1 < *(unsigned char *) s2) ? -1 : +1);
        else if (*s1 == '\0')
            return 0;
    return 0;
}
/*
 Code Reference :  My Previous Submission
 */
int parse_ELF(char *exeFileData, uint64_t exeFileLength,
        struct Exe_Format *exeFormat, programHeader * phdr)
{
    elfHeader *hdr;
    // programHeader *phdr;
    int i;
    hdr = (elfHeader *) exeFileData;
    if (exeFileLength < sizeof(elfHeader)
            || strncmp(exeFileData, "\x7F" "ELF", 4) != 0)
    {
        if (elfDebug)
            printk("Not an ELF executable\n");
        return 1;
    }
    if (hdr->phnum > EXE_MAX_SEGMENTS)
    {
        if (elfDebug)
            printk("Too many segments (%d) in ELF executable\n", hdr->phnum);
        return 1;
    }
    if (exeFileLength < hdr->phoff + (hdr->phnum * sizeof(programHeader)))
    {
        if (elfDebug)
            printk("Not enough room for program header\n");
        return 1;
    }
    //  printk("\nGetting from Elf:%d",hdr->phnum);
    exeFormat->numSegments = hdr->phnum;
    exeFormat->entryAddr = hdr->entry;
    phdr = (programHeader *) (exeFileData + hdr->phoff);
    for (i = 0; i < hdr->phnum; ++i)
    {
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        segment->offsetInFile = phdr[i].offset;
        segment->lengthInFile = phdr[i].fileSize;
        segment->sizeInMemory = phdr[i].memSize;
        segment->vaddr = phdr[i].paddr;
        if (segment->lengthInFile > segment->sizeInMemory)
        {
            if (elfDebug)
                printk(
                        "Segment %d: length in file (%lu) exceeds size in memory (%lu)\n",
                        i, segment->lengthInFile, segment->sizeInMemory);
            return 1;
        }
    }
    return 0;
}
void print_EXEformat(struct Exe_Format* exe_format)
{
    int i = 0;
    //  printk("\n Num Segment of Exe Format : %d", exe_format->numSegments);
    for (; i < exe_format->numSegments; i++)
    {
        //printk("\n Segments: %d : %d: %d: %d",exe_format->segmentList[i].offsetInFile,exe_format->segmentList[i].lengthInFile,exe_format->segmentList[i].startAddress,exe_format->segmentList[i].sizeInMemory );
    }
}
void print_HDRformat(struct Exe_Format* exe_format, programHeader* pdr)
{
    // printk("\n Phdr address %x ",pdr);
    int i = 0;
    // printk("\n Num Segment of Exe Format : %d", exe_format->numSegments);
    for (; i < exe_format->numSegments; i++)
    {
        //    printk("\n Segments: %x : %x: %x: %x :%x :%x :%x : %x ",pdr[i].type,pdr[i].offset,pdr[i].vaddr,pdr[i].paddr,pdr[i].fileSize,pdr[i].memSize,pdr[i].flags,pdr[i].alignment);
    }
}
