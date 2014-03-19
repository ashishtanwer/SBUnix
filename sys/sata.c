#include <defs.h>
#include <stdio.h>
#include<pci.h>
#include<sata.h>
#include<vfs.h>
#include<util.h>
#include<ahci.h>
//void initialize_superblock(){}
/*************************************************************/
int sata_fs_count = 0;
uint64_t superblock_start_addr_p;
uint64_t superblock_start_addr;
superblock* superblock_fs;
uint64_t inode_mgmt_buffer_p;
uint64_t inode_mgmt_buffer;
uint64_t data_mgmt_buffer_p;
uint64_t data_mgmt_buffer;
satafs_entry sata_fs[100];
char buf[512];
struct file kern_vfs_dir;
struct file kern_vfs_fd;
struct file sata_vfs_fd;
/*************************************************************/
/************** FS MANAGEMENT FUNCTIONS *********************/
void kern_init_desc()
{
    int i = 0;
    kern_vfs_dir.address_tarfs_loc = 0x0;
    kern_vfs_fd.address_tarfs_loc = 0x0;
    kern_vfs_dir.inode_num = 0;
    kern_vfs_fd.inode_num = 0;
    kern_vfs_dir.location = DEFAULT_LOC;
    kern_vfs_fd.location = DEFAULT_LOC;
    kern_vfs_dir.perm = 0;
    kern_vfs_fd.perm = 0;
    kern_vfs_dir.size = 0;
    kern_vfs_fd.size = 0;
    for (i = 0; i < 10; i++)
    {
        kern_vfs_dir.sector_loc[i] = 0;
        kern_vfs_fd.sector_loc[i] = 0;
    }
    kern_vfs_dir.offset = 0;
    kern_vfs_fd.offset = 0;
    kern_vfs_dir.next = 0;
    kern_vfs_fd.next = 0;
    kern_vfs_dir.tarfs_table_index = 0;
    kern_vfs_fd.tarfs_table_index = 0;
    kern_vfs_dir.type = DEFAULT_TYPE;
    kern_vfs_fd.type = DEFAULT_TYPE;
    kern_vfs_dir.filename[0] = '\0';
    kern_vfs_fd.filename[0] = '\0';
}
int get_free_inode_block()
{
    int *bitmap_start = (int *) (superblock_fs->free_inode_block);
    int *bitmap_end = (int *) ((int *) superblock_fs->free_inode_block + 4);
    int *temp = bitmap_start;
    int inode_num = 0;
    while (temp < bitmap_end)
    {
        int temp1 = *temp;
        int j;
        for (j = 0; j < 32; j++)
        {
            int temp2;
            temp2 = temp1;
            temp2 &= (1 << j);
            if (temp2 == 0)
            {
                temp1 |= (1 << j);
                *temp = temp1;
                return ++inode_num;
            }
            inode_num++;
        }
        temp++;
    }
    return -1;
}
void set_inode_block_free(int inode_num)
{
    int inode_array_index = inode_num / 32;
    int bit_index = inode_num % 32;
    superblock_fs->free_inode_block[inode_array_index] &= ~(1 << bit_index);
}
int get_free_data_block()
{
    int *bitmap_start = (int *) superblock_fs->free_data_block;
    int *bitmap_end = (int *) (superblock_fs->free_data_block + 1272);
    int *temp = bitmap_start;
    int data_num = 0;
    while (temp < bitmap_end)
    {
        int temp1 = *temp;
        int j;
        for (j = 0; j < 32; j++)
        {
            int temp2;
            temp2 = temp1;
            temp2 &= (1 << j);
            if (temp2 == 0)
            {
                temp1 |= (1 << j);
                *temp = temp1;
                return ++data_num;
            }
            data_num++;
        }
        temp++;
    }
    return -1;
}
void set_data_block_free(int data_num)
{
    int inode_array_index = data_num / 32;
    int bit_index = data_num % 32;
    superblock_fs->free_data_block[inode_array_index] &= ~(1 << bit_index);
}
int get_inode_sector(int inode_num)
{
    if (inode_num % INODES_PER_SECTOR != 0)
        return INODE_S + inode_num / INODES_PER_SECTOR;
    else
        return INODE_S + inode_num / INODES_PER_SECTOR - 1;
}
int get_data_sector(int inode_num)
{
    return inode_num + DATA_S - 1;
}
void write_inode(inode *i)
{
    int inode_sector = get_inode_sector(i->inode_num);
    //printk("[[%d]]", i->sector_loc[0]);
    write_port(&abar->ports[0], inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
    read_port(&abar->ports[0], inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
    inode *write_inode = (inode *) inode_mgmt_buffer;
    write_inode += (i->inode_num - 1) % 3;
    write_inode = i;
    printk("[[%d]]", write_inode->sector_loc[0]);
    write_port(&abar->ports[0], inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
}
inode* read_inode(int inode_num)
{
    int inode_sector = get_inode_sector(inode_num);
    //printk("%d", inode_sector);
    read_port(&abar->ports[0], inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
    inode *inode_i = (inode *) inode_mgmt_buffer;
    inode_i += (inode_num - 1) % 3;
    return inode_i;
}
void write_superblock()
{
    write_port(&abar->ports[0], SUPERBLOCK_S, 0, SUPERBLOCK_SIZE,
            (uint64_t) superblock_start_addr_p);
}
void write_data(int sector_num, uint64_t buf)
{
    write_port(&abar->ports[0], sector_num, 0, 1, buf);
}
/********************* TABLE RELATED MANAGEMENT *****************/
int upto_last_slash_len(char *full_path)
{
    int len = strlen(full_path);
    int i = 0;
    char *tmp = full_path + len - 1;
    while ((*tmp) != '/')
    {
        tmp--;
        i++;
    }
    if ((*tmp) != '/')
        return -1;
    len = (len - i - 1);
    return len;
}
// full_path="/hiphop/abc/a" --> a is directory
// full_path="/hiphop/abc/a" --> a is file
int find_parent_index(char *full_path)
{
    int j = 0;
    int slash_len = 0;
    char cmp_buf[100];
    slash_len = upto_last_slash_len(full_path);
// there will be atleast /hiphop
    if (slash_len < 8)
        return DEFAULT_INDEX_FOR_TABLE;
    //	return -1;
    strncpy(cmp_buf, full_path, (slash_len - 1));
    for (j = 0; j < sata_fs_count; j++)
    {
        if (strcmp(cmp_buf, sata_fs[j].name) == 0)
        {
            if (sata_fs[j].typeflag == DIRECTORY)
            {
                return j;
            }
        }
    }
    return DEFAULT_INDEX_FOR_TABLE;
}
// full_path="/hiphop/abc/a" --> a is directory
// full_path="/hiphop/abc/a" --> a is file
void add_inode_to_table(inode *inode_i)
{
    satafs_entry table_entry;
    table_entry.inode_num = inode_i->inode_num;
    strcpy(table_entry.name, inode_i->filename);
    table_entry.size = inode_i->size;
    table_entry.typeflag = inode_i->type;
    table_entry.par_ind = find_parent_index(table_entry.name);
    sata_fs[sata_fs_count++] = table_entry;
}
/*
 Code Reference :  Adopted from code reference from Shashank
 */
void init_satatable()
{
    superblock_start_addr_p =
            ((uint64_t) pages_for_ahci_start + (20 * 4096) / 8);
    superblock_start_addr = KERNBASE + superblock_start_addr_p;
    inode_mgmt_buffer_p = ((uint64_t) pages_for_ahci_start + (23 * 4096) / 8);
    inode_mgmt_buffer = KERNBASE
            + ((uint64_t) pages_for_ahci_start + (23 * 4096) / 8);
    data_mgmt_buffer_p = ((uint64_t) pages_for_ahci_start + (24 * 4096) / 8);
    data_mgmt_buffer = KERNBASE
            + ((uint64_t) pages_for_ahci_start + (24 * 4096) / 8);
    read_port(&abar->ports[0], 1, 0, 20, (uint64_t) superblock_start_addr_p);
    superblock_fs = (superblock *) superblock_start_addr;
    printk("\n[superblock] [%x]", (uint64_t) superblock_start_addr_p);
    printk("\n[superblock] [%x]", (uint64_t) superblock_start_addr);
    printk("\n[magic] [%d]", superblock_fs->magic_no);
    printk("\n[name] [%s]", superblock_fs->fs_type);
    printk("\n[inode_free] [%x]", superblock_fs->free_inode_block[0]);
    ///////////////////////////////////// TABLE INITIALISE ///////////////////////////////
    int i, j; //,slash_len;
    inode* inode_i;
    satafs_entry table_entry;
    //char cmp_buf[100];
    sata_fs_count = 0;
    for (i = INODE_S; i < INODE_E; i++)
    {
        read_port(&abar->ports[0], i, 0, 1, (uint64_t) inode_mgmt_buffer_p);
        inode_i = (inode *) inode_mgmt_buffer;
        for (j = 0; j < 3; j++)
        {
            if ((*((char *) inode_i->filename)) == '\0')
                continue;
            table_entry.inode_num = inode_i->inode_num;
            strcpy(table_entry.name, inode_i->filename);
            table_entry.size = inode_i->size;
            table_entry.typeflag = inode_i->type;
            table_entry.par_ind = DEFAULT_INDEX_FOR_TABLE;
            table_entry.number_of_dentry = 0;
            //                printk("\n kksata fs count=%d",sata_fs_count);
            sata_fs[sata_fs_count++] = table_entry;
            //	printk("\n %d", inode_i->inode_num);
            //		printk("[%s]",inode_i->filename);
            inode_i++;
        }
    }
    printk("Initiaize sata successful\n");
}
void print_satatable()
{
    int i;
    printk("\n PRINTING TABLE");
    printk("\n Total Entries = [%d]", sata_fs_count - 1);
    printk("\n [inode_num] [size] [typeflag] [index] [par_ind] [name]");
    for (i = 0; i < sata_fs_count; i++)
        //printk("\n [%d] [%d] [%d] [%d] [%d] [%s]",sata_fs[i].inode_num,sata_fs[i].size,sata_fs[i].typeflag,i,sata_fs[i].par_ind,sata_fs[i].name);
        printk("\n[%d] [%d] [%s]", i, sata_fs[i].par_ind, sata_fs[i].name);
}
/**********************************************************/
int lookup_fs(char *name, int flag)
{
    //satafs_entry table_entry;
    int i = 0;
    while (i < sata_fs_count)
    {
        //table_entry = sata_fs[i];
        //	if(strlen(table_entry.name) == 0)
        //	break;
        if (strcmp(sata_fs[i].name, name) == 0 && sata_fs[i].typeflag == flag)
            return sata_fs[i].inode_num;
        i++;
    }
    return 0;
}
/**************** SYSCALLS RELATED**************************/
/*
 Code Reference :  Adopted from code reference from Shashank
 */
struct file * file_open(char *filename, int flag)
{
    int inode_num = lookup_fs(filename, flag);
    inode * inode_i;
    int i = 0;
    kern_init_desc();
    if (inode_num != 0)
    {
        inode_i = read_inode(inode_num);
        if (flag == DIRECTORY)
            printk("\nDIrectory EXISTS");
        else
            printk("\nFile EXISTS");
        printk("\nsize %d\n", inode_i->size);
    }
    else if (inode_num == 0 && flag == FILE_TYPE)
    {
        int dir_index = find_parent_index(filename);
        if (dir_index == -1)
        {
            printk(
                    "\n Required Directory doesn't Exist :: Create one and try again");
            return NULL;
        }
        int new_inode_number = get_free_inode_block();
        int new_inode_sector = get_inode_sector(new_inode_number);
        printk("\n %d %d", new_inode_number, new_inode_sector);
        read_port(&abar->ports[0], new_inode_sector, 0, 1,
                (uint64_t) inode_mgmt_buffer_p);
        printk("\n %x %x", inode_mgmt_buffer, inode_mgmt_buffer_p);
        inode_i = (inode *) inode_mgmt_buffer;
        inode_i += (new_inode_number - 1) % 3;
        inode_i->inode_num = new_inode_number;
        strcpy(inode_i->filename, filename);
        inode_i->size = 0;
        inode_i->type = FILE_TYPE;
        inode_i->perm = 0;
        for (i = 0; i < 10; i++)
            inode_i->sector_loc[i] = 0;
        sata_fs[dir_index].number_of_dentry += 1;
        add_inode_to_table(inode_i);
        write_superblock();
        write_port(&abar->ports[0], new_inode_sector, 0, 1,
                (uint64_t) inode_mgmt_buffer_p);
    }
    else if (inode_num == 0 && flag == DIRECTORY)
    {
        printk("\n NO Directory Exists");
        return NULL;
    }
    else
        return NULL;
    sata_vfs_fd.inode_num = inode_i->inode_num;
    strcpy(sata_vfs_fd.filename, inode_i->filename);
    sata_vfs_fd.perm = inode_i->perm;
    sata_vfs_fd.size = inode_i->size;
    sata_vfs_fd.type = inode_i->type;
    sata_vfs_fd.location = FS_LOC;
    for (i = 0; i < 10; i++)
        sata_vfs_fd.sector_loc[i] = inode_i->sector_loc[i];
    for (i = 0; i < sata_fs_count; i++)
    {
        if (inode_i->inode_num == sata_fs[i].inode_num && inode_i->type == flag)
        {
            sata_vfs_fd.tarfs_table_index = i;
            break;
        }
    }
    return &sata_vfs_fd;
}
/********************************************************************/
/*
 Code Reference :  Adopted from code reference from Shashank
 */
int file_write(struct file *fd, char * buf, int size)
{
    inode * inode_i;
    int len = 0;
    int offset = fd->offset;
    int offset_in_sector;
    int sector_index;
    int sector_num;
    int free_data_block_no;
    int free_data_block_sector;
    int page_no = 0;
    int i = 0;
    if (fd->inode_num != 0)
    {
        inode_i = read_inode(fd->inode_num);
        sector_index = (fd->offset / 512);
        sector_num = inode_i->sector_loc[sector_index];
        while (len < size)
        {
            offset_in_sector = ((offset - 1) % 512);
            sector_index = (offset / 512);
            sector_num = inode_i->sector_loc[sector_index];
            if (sector_num == 0)
            {
                free_data_block_no = get_free_data_block();
                free_data_block_sector = get_data_sector(free_data_block_no);
                inode_i->sector_loc[page_no++] = free_data_block_sector;
                sector_num = free_data_block_sector;
            }
            if (offset_in_sector != 0
                    || (offset_in_sector == 0 && (size - len) < 512))
                read_port(&abar->ports[0], sector_num, 0, 1,
                        (uint64_t) data_mgmt_buffer_p);
            char *tmp = (char *) data_mgmt_buffer;
            while (len < size && offset_in_sector <= 512)
            {
                tmp[((offset - 1) % 512)] = buf[len];
                len++;
                offset++;
                offset_in_sector++;
            }
            write_port(&abar->ports[0], sector_num, 0, 1,
                    (uint64_t) data_mgmt_buffer_p);
        }
        inode_i->size += len;
        write_inode(inode_i);
        write_superblock();
        for (i = 0; i < sata_fs_count; i++)
        {
            if (inode_i->inode_num == sata_fs[i].inode_num)
            {
                sata_fs[i].size = inode_i->size;
                break;
            }
        }
    }
    else
    {
        printk("\nFile doesnt exist :: Try creating file first");
        return 0;
    }
    return len;
}
/*********************************************************************************/
int file_seek(struct file *fd, int offset, int whence)
{
    if (fd == NULL)
        return 0;
    int new_pos = offset + whence;
    if (new_pos < 0 || fd->size < new_pos)
        return 0;
    fd->offset = offset + whence;
    return 1;
}
/*
 Code Reference :  Adopted from code reference from Shashank
 */
int file_read(struct file *fd, char *ptr, longlong_t nitems)
{
    if (fd == NULL)
        return 0;
    inode * inode_i;
    if (fd->inode_num != 0)
    {
        inode_i = read_inode(fd->inode_num);
        printk("\n[%s]", inode_i->filename);
        int no_of_sectors = (nitems + fd->offset) / 512 + 1;
        int i;
        int no_of_sectors_to_skip = fd->offset / 512;
        int bytes_skip_in_fst_sec = fd->offset % 512;
        int start_sector_index = no_of_sectors_to_skip;
        int bytes_read_in_lst_sec = (fd->offset + nitems) % 512;
        int last_sector_index = start_sector_index + no_of_sectors - 1;
        int buffer_size_increament = 0;
        if (last_sector_index > 10)
            return 0;
        for (i = start_sector_index; i <= last_sector_index; i++)
        {
            if (inode_i->sector_loc[i] == 0)
                return 0;
            read_port(&abar->ports[0], inode_i->sector_loc[i], 0, 1,
                    (uint64_t) data_mgmt_buffer_p);
            if (i == start_sector_index && i != last_sector_index)
            {
                strncpy(ptr + buffer_size_increament,
                        (char *) data_mgmt_buffer + bytes_skip_in_fst_sec,
                        512 - bytes_skip_in_fst_sec);
                buffer_size_increament += 512 - bytes_skip_in_fst_sec;
            }
            else if (i == start_sector_index && i == last_sector_index)
            {
                strncpy(ptr + buffer_size_increament,
                        (char *) data_mgmt_buffer + bytes_skip_in_fst_sec,
                        nitems);
                buffer_size_increament += nitems;
            }
            else if (i == last_sector_index)
            {
                strncpy(ptr + buffer_size_increament, (char *) data_mgmt_buffer,
                        bytes_read_in_lst_sec);
                buffer_size_increament += bytes_read_in_lst_sec;
            }
            else
            {
                strncpy(ptr + buffer_size_increament, (char *) data_mgmt_buffer,
                        512);
                buffer_size_increament += 512;
            }
        }
    }
    else
    {
        printk("Error !!!");
        return 0;
    }
    return nitems;
}
/*
 Code Reference :  Adopted from code reference from Shashank
 */
int make_dir(char * name)
{
    if (lookup_fs(name, DIRECTORY))
    {
        printk("\nAlready exist dir");
        return 0;
    }
    inode * inode_i;
    int new_inode_number;
    int new_inode_sector;
    int dir_index = find_parent_index(name);
    if (dir_index == -1)
    {
        printk(
                "\n Required PARENT Directory doesn't Exist :: Create one and try again");
        return 0;
    }
    new_inode_number = get_free_inode_block();
    new_inode_sector = get_inode_sector(new_inode_number);
    printk("\n %d %d", new_inode_number, new_inode_sector);
    read_port(&abar->ports[0], new_inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
    inode_i = (inode *) inode_mgmt_buffer;
    inode_i += (new_inode_number - 1) % 3;
    inode_i->inode_num = new_inode_number;
    strcpy(inode_i->filename, name);
    inode_i->size = 0;
    inode_i->type = DIRECTORY;
    inode_i->perm = 0;
    int i = 0;
    for (i = 0; i < 10; i++)
        inode_i->sector_loc[i] = 0;
    write_superblock();
    write_port(&abar->ports[0], new_inode_sector, 0, 1,
            (uint64_t) inode_mgmt_buffer_p);
    printk("\n sata_fs_count = [%d]", sata_fs_count);
    add_inode_to_table(inode_i);
    return 1;
}
int compare(char *src, char*dest, int len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        if (*src == *dest)
        {
            src++;
            dest++;
            i++;
        }
        else
            return 0;
    }
    return 1;
}
/*
 Code Reference :  Adopted from code reference from Shashank
 */
int dir_read(struct file * dir, char *buf, int size)
{
    int i = 0, k = 0, j = 0, z = 0;
    //int par_ind = 0;
    int count = 0;
    int flag = 0;
    for (i = 0; i < sata_fs_count; i++)
    {
        for (z = 0; dir->filename[z] != '\0'; z++)
        {
            if (dir->filename[z + 1] == '\0')
            {
                flag = 1;
                break;
            }
            if (dir->filename[z] != sata_fs[i].name[z])
            {
                break;
            }
        }
        if (flag == 1)
        {
            for (j = 0; dir->filename[j] != '\0'; j++)
            {
                //i=i;
            }
            for (k = j; sata_fs[i].name[k] != '\0'; k++)
            {
                buf[count++] = sata_fs[i].name[k];
                if (count == size - 1)
                {
                    buf[count] = '\0';
                    return 1;
                }
            }
            buf[count++] = ',';
            //k=k+1;
            flag = 0;
        }
    }
    buf[count] = '\0';
    return 1;
}
