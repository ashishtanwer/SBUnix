#ifndef _TARFS_H
#define _TARFS_H

#include<defs.h>
#include<process.h>

extern char _binary_tarfs_start;
extern char _binary_tarfs_end;

struct posix_header_ustar
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char pad[12];
};

struct file_info
{

    uint64_t file_start;
    uint64_t file_current;

    int end_of_file;

};

struct dir_info
{

    uint64_t dir_start;
    uint64_t dir_current;
    int end_of_dir;
    char dir_path[100];
    char previous_file[100];

};

/*
 struct file_descriptor{


 uint8_t count;
 struct file_info file_buf[MAX_PROCESS];
 // uint64_t processid[MAX_PROCESS];
 // uint64_t last;
 uint8_t position[MAX_PROCESS];



 };

 */

char* get_name_file(uint64_t);

uint64_t get_next_pointer(uint64_t);

uint64_t get_dir_start_address(int);

void strcpy(const char *, char *);

uint64_t get_file_start_address(int);

void init_tarfs();

int get_file_id();

int get_dir_id();

uint64_t get_address_dir(char *, char *, char*);

uint64_t get_address_file(char *, char *, char*);

int open_dir(char *);

struct dir_info* get_dir_info(int);

int open_file(char *);

int octal_decimal(int);

void print_posix_header(struct posix_header_ustar *);

int read_tarfs_file(char *, char *, char*);
int get_elf_file(char *, char*, char*);

int matchString(char *, char *);

int atoi(char *);

uint64_t map_temp();
void map_exe_format();

void switch0to3(uint64_t);
void map_exe(struct mm_struct *);

uint64_t get_entry_address();

void swith0to3(uint64_t);

#endif
