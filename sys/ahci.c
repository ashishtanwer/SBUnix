#include <defs.h>
#include <stdio.h>
#include <pci.h>
#include <ahci.h>
#include <vmm.h>
#define SATA_SIG_ATA    0x00000101  // SATA drive
#define SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101  // Port multiplier
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SATAPI 4
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1
#define AHCI_BASE   0x400000    // 4M
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES   (1 << 30)       /* TFES - Task File Error Status */
#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX     0x35
char test[20] = "6317065029";
void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
int find_cmdslot(HBA_PORT *port);
HBA_MEM *abar;
extern char fs_buf[1024];
/*
 Code Reference :  OSDEV
 */
void handler_ahci_routine()
{
    printk("AHCI INTERRUPT HANDLER\n");
    if (abar->ports[0].is & HBA_PxIS_TFES)
        printk("Read disk error\n");
    printk("\n TFD=[%d]", ((HBA_PORT *) &abar->ports[0])->tfd);
    printk("\nSSTS =[%d]", ((HBA_PORT *) &abar->ports[0])->ssts);
    printk("\nIE=[%d]", ((HBA_PORT *) &abar->ports[0])->ie);
    printk("\nSERR%d", ((HBA_PORT *) &abar->ports[0])->serr);
    printk("\nIS%d", ((HBA_PORT *) &abar->ports[0])->is);
    abar->ports[0].is = 0xffff;
    while (1)
        ;
}
/*
 Code Reference : OSDEV
 */
int read_port(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count,
        QWORD buf)
{
    port->is = 0xffff; // Clear pending interrupt bits
    int slot = find_cmdslot(port);
    if (slot == -1)
        return 0;
    uint64_t addr = 0;
    addr = (((addr | port->clbu) << 32) | port->clb);
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) (KERNBASE + addr);
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD); // Command FIS size
    cmdheader->w = 0; // Read from device
    cmdheader->c = 1; // Read from device
    cmdheader->p = 1; // Read from device
    cmdheader->prdtl = (WORD) ((count - 1) >> 4) + 1; // PRDT entries count
    addr = 0;
    addr = (((addr | cmdheader->ctbau) << 32) | cmdheader->ctba);
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*) (KERNBASE + addr);
    int i = 0;
    for (i = 0; i < cmdheader->prdtl - 1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (DWORD) (buf & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbau = (DWORD) ((buf << 32) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1; // 8K bytes
        cmdtbl->prdt_entry[i].i = 0;
        buf += 4 * 1024; // 4K words
        count -= 16; // 16 sectors
    }
    cmdtbl->prdt_entry[i].dba = (DWORD) (buf & 0xFFFFFFFF);
    cmdtbl->prdt_entry[i].dbau = (DWORD) ((buf << 32) & 0xFFFFFFFF);
    cmdtbl->prdt_entry[i].dbc = count << 9; // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 0;
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_READ_DMA_EX;
    cmdfis->lba0 = (BYTE) startl;
    cmdfis->lba1 = (BYTE) (startl >> 8);
    cmdfis->lba2 = (BYTE) (startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode
    cmdfis->lba3 = (BYTE) (startl >> 24);
    cmdfis->lba4 = (BYTE) starth;
    cmdfis->lba5 = (BYTE) (starth >> 8);
    cmdfis->countl = count & 0xff;
    cmdfis->counth = count >> 8;
    //printk("[slot]{%d}", slot);
    port->ci = 1; // Issue command
    while (1)
    {
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES)
        { // Task file error
            printk("Read disk error\n");
            return 0;
        }
    }
    if (port->is & HBA_PxIS_TFES)
    {
        printk("Read disk error\n");
        return 0;
    }
    int k = 0;
    while (port->ci != 0)
    {
        printk("[%d]", k++);
    }
    return 1;
}
/*
 Code Reference : OSDEV
 */
int write_port(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count,
        QWORD buf)
{
    printk("Inside writeport \n ");
    port->is = 0xffff; // Clear pending interrupt bits
    int slot = find_cmdslot(port);
    if (slot == -1)
        return 0;
    uint64_t addr = 0;
    addr = (((addr | port->clbu) << 32) | port->clb);
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) (KERNBASE + addr);
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD); // Command FIS size
    cmdheader->w = 1; // Read from device
    cmdheader->c = 1; // Read from device
    cmdheader->p = 1; // Read from device
    cmdheader->prdtl = (WORD) ((count - 1) >> 4) + 1; // PRDT entries count
    addr = 0;
    addr = (((addr | cmdheader->ctbau) << 32) | cmdheader->ctba);
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*) (KERNBASE + addr);
    int i = 0;
    for (i = 0; i < cmdheader->prdtl - 1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (DWORD) (buf & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbau = (DWORD) ((buf << 32) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1; // 8K bytes
        cmdtbl->prdt_entry[i].i = 0;
        buf += 4 * 1024; // 4K words
        count -= 16; // 16 sectors
    }
    cmdtbl->prdt_entry[i].dba = (DWORD) (buf & 0xFFFFFFFF);
    cmdtbl->prdt_entry[i].dbau = (DWORD) ((buf << 32) & 0xFFFFFFFF);
    cmdtbl->prdt_entry[i].dbc = count << 9; // 512 bytes per sector
    cmdtbl->prdt_entry[i].i = 0;
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // Command
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;
    cmdfis->lba0 = (BYTE) startl;
    cmdfis->lba1 = (BYTE) (startl >> 8);
    cmdfis->lba2 = (BYTE) (startl >> 16);
    cmdfis->device = 1 << 6; // LBA mode
    cmdfis->lba3 = (BYTE) (startl >> 24);
    cmdfis->lba4 = (BYTE) starth;
    cmdfis->lba5 = (BYTE) (starth >> 8);
    cmdfis->countl = count & 0xff;
    cmdfis->counth = count >> 8;
    //    printk("[slot]{%d}", slot);
    port->ci = 1; // Issue command
    while (1)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if ((port->ci & (1 << slot)) == 0)
            break;
        if (port->is & HBA_PxIS_TFES)
        { // Task file error
            printk("Read disk error\n");
            return 0;
        }
    }
    if (port->is & HBA_PxIS_TFES)
    {
        printk("Read disk error\n");
        return 0;
    }
    int k = 0;
    while (port->ci != 0)
    {
        printk("[%d]", k++);
    }
    return 1;
}
// To setup command fing a free command list slot
int find_cmdslot(HBA_PORT *port)
{
    DWORD slots = (port->sact | port->ci);
    int num_of_slots = (abar->cap & 0x0f00) >> 8; // Bit 8-12
    int i;
    for (i = 0; i < num_of_slots; i++)
    {
        if ((slots & 1) == 0)
        {
            //		printk("[slot=%d]", i);
            //	if(i==0)
            return i;
            //	break;
        }
        slots >>= 1;
    }
    printk("Cannot find free command list entry\n");
    return -1;
}
// Check device type
static int check_type(HBA_PORT *port)
{
    DWORD ssts = port->ssts;
    BYTE ipm = (ssts >> 8) & 0x0F;
    BYTE det = ssts & 0x0F;
    //printk ("\n ipm %d det %d sig %d", ipm, det, port->sig);
    if (det != HBA_PORT_DET_PRESENT) // Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;
    switch (port->sig)
    {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
    return 0;
}
/*
 Code Reference : OSDEV
 */
void rebase_port(HBA_PORT *port, int portno)
{
    printk("Inside port_rebase\n");
    abar->ghc = (DWORD) (1 << 31);
    abar->ghc = (DWORD) (1 << 0);
    abar->ghc = (DWORD) (1 << 31);
    abar->ghc = (DWORD) (1 << 1);
    int i;
    //printk("in");
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    // Wait until FR (bit14), CR (bit15) are cleared
    while (1)
    {
        if (port->cmd & HBA_PxCMD_FR)
        {
            printk("\nFIS IS RUNNING");
            continue;
        }
        if (port->cmd & HBA_PxCMD_CR)
        {
            printk("\nCMD LIST IS RUNNING");
            continue;
        }
        break;
    }
    port->cmd = port->cmd & 0xffff7fff; //Bit 15
    port->cmd = port->cmd & 0xffffbfff; //Bit 14
    port->cmd = port->cmd & 0xfffffffe; //Bit 0
    port->cmd = port->cmd & 0xfffffff7; //Bit 4
    // printk("\naddress is ssad=%x",pages_for_ahci_start);
    port->clb = (((uint64_t) pages_for_ahci_start & 0xffffffff));
    // printk("\naddress is ssad=%x\n",port->clb);
    port->clbu = 0;
    // printk("\naddress is ssad=%x\n",port->clbu);
    port->fb = (((uint64_t) pages_for_ahci_start + (uint64_t) ((32 << 10) / 8))
            & 0xffffffff);
    // printk("\naddress is ssad=%x\n",port->fb);
    port->fbu =
            ((((uint64_t) pages_for_ahci_start + (uint64_t) ((32 << 10) / 8))
                    >> 32) & 0xffffffff);
    /// printk("\naddress is ssad=%x\n",port->fbu);
    // }
    port->serr = 1; //For each implemented port, clear the PxSERR register, by writing 1 to each implemented location
    port->is = 0; //
    port->ie = 1;
    uint64_t addres = 0;
    addres = (((addres | port->clbu) << 32) | port->clb);
    addres = addres + KERNBASE;
    memset((void *) addres, 0, 1024);
    addres = 0;
    addres = (((addres | port->fbu) << 32) | port->fb);
    addres = addres + KERNBASE;
    memset((void*) addres, 0, 256);
    addres = 0;
    addres = (((addres | port->clbu) << 32) | port->clb);
    addres = (addres + KERNBASE);
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *) addres;
    i = 0;
    printk("port_rebase incomplete1\n");
    for (i = 0; i < 32; i++)
    {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
        cmdheader[i].ctba = (((uint64_t) pages_for_ahci_start
                + (uint64_t) ((40 << 10) / 8) + (uint64_t) ((i << 8) / 8))
                & 0xffffffff);
        cmdheader[i].ctbau =
                ((((uint64_t) pages_for_ahci_start + (uint64_t) ((40 << 10) / 8)
                        + (uint64_t) ((i << 8) / 8)) >> 32) & 0xffffffff);
        addres = 0;
        addres = (((addres | cmdheader[i].ctbau) << 32) | cmdheader[i].ctba);
        addres = addres + KERNBASE;
    }
    printk("port_rebase incomplete2\n");
    while (port->cmd & HBA_PxCMD_CR)
        ;
    printk("port_rebase incomplete3\n");
    printk("%x", port->cmd);
    port->cmd |= HBA_PxCMD_FRE;
    printk("%x", port->cmd);
    port->cmd |= HBA_PxCMD_ST;
    //port->cmd=0x17;
    printk("port_rebase incomplete4\n");
    port->is = 0;
    port->ie = 0xffffffff;
    printk("port_rebase complete\n");
}
/*
 Code Reference : OSDEV
 */
void probe_port(HBA_MEM *abar_temp)
{
    printk("Inside probe_port\n");
    // Search disk in impelemented ports
    DWORD pi = abar_temp->pi;
    int i = 0;
    while (i < 32)
    {
        if (pi & 1)
        {
            int dt = check_type((HBA_PORT *) &abar_temp->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                printk("\nSATA drive found at port %d\n", i);
                abar = abar_temp;
                rebase_port(abar_temp->ports, i);
                printk("\nDONE AHCI INITIALISATION :: PORT REBASE");
                return;
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                //printk("\nSATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                //printk("\nSEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                //printk("\nPM drive found at port %d\n", i);
            }
            else
            {
                //printk("\nNo drive found at port %d\n", i);
            }
        }
        pi >>= 1;
        i++;
    }
    printk("probe_port complete\n");
}
