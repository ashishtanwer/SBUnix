#ifndef _VFS_H
#define _VFS_H
//#include<.h>
#include<defs.h>
/**********TARFS + FS + N/W ************/
#define TARFS_LOC 0 
#define FS_LOC 1 
#define DEFAULT_LOC -1 

#define DEFAULT_TARFS_TABLE_INDEX -1 

#define DIRECTORY 5
#define FILE_TYPE 0
#define DEFAULT_TYPE  9

struct file
{
    uint64_t address_tarfs_loc;
    int inode_num;
    int location;
    int perm;
    int size;
    int sector_loc[10];
    int offset;
    int next;
    int tarfs_table_index;
    char type;
    char filename[100];
};

typedef struct
{
    char name[100];
    int type;
    int location;
    int tarfs_table_index;
    int inode_num;
} dirent;

extern struct file kern_vfs_dir;
extern struct file kern_vfs_fd;
void kern_init_desc();

extern struct file vfs_dir;
extern struct file vfs_fd;
void init_desc();

/**************************** OPERATONS ON TARFS + FS *********************************/
struct file *vfs_opendir(char * name);
int readdir_ls(struct file *dirp, char*, int);
int vfs_closedir(struct file *dirp);
struct file *fopen(char *filename_full_path);
uint32_t fread(struct file *fd, char *buf, uint32_t size);
int fseek(struct file *fd, int offset, int whence);
int fwrite(struct file *fd, char *buf, int size);
int fclose(struct file *fd);
int make_dir_vfs(char *name);

/******************************************************************************************/
#endif
