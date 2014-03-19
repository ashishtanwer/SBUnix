#ifndef _SATA_H
#define _SATA_H

#include<defs.h>
#include<vfs.h>
/************************ FS TYPES *************************/
#define DIRECTORY 5
#define FILE_TYPE 0

/************************ FS ORGANIZATION ****************************/
#define SUPERBLOCK_SIZE 20
#define SUPERBLOCK_S 1
#define SUPERBLOCK_E 20
#define INODE_S 23
#define INODE_E 151
#define RESERVE_I1_S 152
#define RESERVE_I1_E 160
#define SUPERBLOCK_COPY_S 161
#define SUPERBLOCK_COPY_E 180
#define RESERVE_I2_S 181
#define RESERVE_I2_E 190
#define DATA_S 191
#define DATA_E 40704
#define SECTOR_SIZE 512
#define INODES_PER_SECTOR 3
#define DEFAULT_INDEX_FOR_TABLE 999

/*********************************************************************/

/************************** FS STRUCTURES *****************************/
typedef struct
{
    char fs_type[10];
    int size;
    int magic_no;
    int inode_start_sector;
    int data_start_sector;
    int free_inode_block[4];
    int free_data_block[1272];
} superblock;

typedef struct
{
    int inode_num;
    char filename[100];
    int perm;
    int size;
    char type;
    int number_of_dentry;
    int sector_loc[10];
} inode;

// each sector can contain 512/(100+4)=4 entry
typedef struct
{
    char filename[100];
    int inode_num;
} dentry;

typedef struct
{
    char inode_num;
    char name[100];
    int size;
    int typeflag;
    int par_ind;
    int number_of_dentry;
} satafs_entry;

extern satafs_entry sata_fs[100];
extern int sata_fs_count;

extern uint64_t superblock_start_addr_p;
extern uint64_t superblock_start_addr;
extern superblock* superblock_fs;
extern uint64_t inode_mgmt_buffer_p;
extern uint64_t inode_mgmt_buffer;
extern uint64_t data_mgmt_buffer_p;
extern uint64_t data_mgmt_buffer;

/*************************************************************************************************/

/************** FS MANAGEMENT FUNCTIONS *********************/
int get_free_inode_block();
void set_inode_block_free(int inode_num);
int get_free_data_block();
void set_data_block_free(int data_num);
int get_inode_sector(int inode_num);
int get_data_sector(int inode_num);
void write_inode(inode *i);
inode* read_inode(int inode_num);
void write_superblock();
void write_data(int sector_num, uint64_t buf);
int upto_last_slash_len(char *full_path);
int find_parent_index(char *full_path);
void add_inode_to_table(inode *inode_i);
void init_satatable();
void print_satatable();
/**************************************************************/

int lookup_fs(char *name, int flag);
void initialize_superblock();
/******************* SYSCALL RELATED *********************/

struct file * file_open(char *filename, int flag);
int file_write(struct file *fd, char * buf, int size);
int file_seek(struct file *fd, int offset, int whence);
int file_read(struct file *fd, char *ptr, longlong_t nitems);
int make_dir(char * name);
int dir_read(struct file *dd, char* buf, int size);

/********************************************************/
#endif
