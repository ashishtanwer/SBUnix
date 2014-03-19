#include <defs.h>
#include <pci.h>
#include <e1000.h>
struct e1000_rx_desc recv_desc[64];
struct recv_data_buffer recv_buf[64];
char testbuf[125];
char *ffff = testbuf;
struct e1000_tx_desc trans_desc[64];
struct trans_data_buffer trans_buf[64];
void handler_e1000_routine(void)
{
    //printk("\n Getting e1000 interrupt");
}
/*
 Code Reference : Adopted from code reference of Shashank
 */
void e1000_initialise(struct pci_func *e1000)
{
    uint64_t start = (uint64_t) (e1000->start_virtual_address);
    (*((uint32_t *) (start + E1000_CTRL))) |= E1000_CTRL_ASDE;
    (*((uint32_t *) (start + E1000_CTRL))) |= E1000_CTRL_SLU;
    (*((uint32_t *) (start + E1000_CTRL))) &= (~E1000_CTRL_LRST);
    (*((uint32_t *) (start + E1000_CTRL))) &= (~E1000_CTRL_PHY_RST);
    (*((uint32_t *) (start + E1000_CTRL))) &= (~E1000_CTRL_ILOS);
    (*((uint32_t *) (start + E1000_FCAH))) = 0x0;
    (*((uint32_t *) (start + E1000_FCAL))) = 0x0;
    (*((uint32_t *) (start + E1000_FCT))) = 0x0;
    (*((uint32_t *) (start + E1000_FCTTV))) = 0x0;
    (*((uint32_t *) (start + E1000_CTRL))) &= (~E1000_CTRL_RFCE);
    (*((uint32_t *) (start + E1000_CTRL))) &= (~E1000_CTRL_TFCE);
#define E1000_RAL       0x05400  /* Receive Address LOW - RW Array */
#define E1000_RAH       0x05404  /* Receive Address High - RW Array */
    (*((uint32_t *) (start + E1000_RAL))) = 0x52540012;
    (*((uint32_t *) (start + E1000_RAH))) = 0x3456;
    (*((uint32_t *) (start + E1000_IMS))) |= E1000_IMS_RXT0;
    (*((uint32_t *) (start + E1000_IMS))) |= E1000_IMS_RXO;
    (*((uint32_t *) (start + E1000_IMS))) |= E1000_IMS_RXDMT0;
    (*((uint32_t *) (start + E1000_IMS))) |= E1000_IMS_RXSEQ;
    (*((uint32_t *) (start + E1000_IMS))) |= E1000_IMS_LSC;
    int i = 0;
    for (i = 0; i < 64; i++)
        recv_desc[i].buffer_addr = (uint64_t) (((char *) (recv_buf[i].buf))
                - 0xffffffff80000000);
    recv_buf[0].buf[0] = 's';
    recv_buf[0].buf[1] = 's';
    recv_buf[0].buf[2] = 's';
    recv_buf[0].buf[3] = 's';
    recv_buf[0].buf[4] = 's';
    recv_buf[0].buf[5] = '\0';
    uint64_t recv_buf_phy = (uint64_t) (((char *) recv_desc)
            - 0xffffffff80000000);
    (*((uint32_t *) (start + E1000_RDBAL))) = (uint32_t) (recv_buf_phy
            & 0xffffffff);
    (*((uint32_t *) (start + E1000_RDBAH))) =
            (uint32_t) ((((uint64_t) recv_buf_phy) >> 32) & 0xffffffff);
    (*((uint32_t *) (start + E1000_RDLEN))) = 256;
    (*((uint32_t *) (start + E1000_RDH))) = 0;
    (*((uint32_t *) (start + E1000_RDT))) = 0;
    (*((uint32_t *) (start + E1000_RCTL))) |= E1000_RCTL_EN;
    (*((uint32_t *) (start + E1000_RCTL))) &= (~E1000_RCTL_LPE);
    (*((uint32_t *) (start + E1000_RCTL))) &= (~(0x3 << 6)); //(E1000_RCTL_LBM_NO);
    (*((uint32_t *) (start + E1000_RCTL))) |= (E1000_RCTL_RDMTS_EIGTH);
    (*((uint32_t *) (start + E1000_RCTL))) |= (E1000_RCTL_MO_3);
    (*((uint32_t *) (start + E1000_RCTL))) |= (E1000_RCTL_BAM);
    (*((uint32_t *) (start + E1000_RCTL))) &= (~(0x3 << 16));
    (*((uint32_t *) (start + E1000_RCTL))) &= (~(0x1 << 25));
    (*((uint32_t *) (start + E1000_RCTL))) |= (E1000_RCTL_SECRC);
    i = 0;
    for (i = 0; i < 64; i++)
    {
        trans_desc[i].buffer_addr = (uint64_t) (((char *) (trans_buf[i].buf))
                - 0xffffffff80000000);
        trans_desc[i].lower.flags.length = 100;
        trans_desc[i].lower.flags.cmd = 25;
    }
    uint64_t trans_buf_phy = (uint64_t) (((char *) &trans_desc[0])
            - 0xffffffff80000000);
    (*((uint32_t *) (start + E1000_TDBAL))) = (uint32_t) (trans_buf_phy
            & 0xffffffff);
    (*((uint32_t *) (start + E1000_TDBAH))) =
            (uint32_t) ((((uint64_t) trans_buf_phy) >> 32) & 0xffffffff);
    (*((uint32_t *) (start + E1000_TDLEN))) = 256;
    (*((uint32_t *) (start + E1000_TDH))) = 0;
    (*((uint32_t *) (start + E1000_TDT))) = 0;
    (*((uint32_t *) (start + E1000_TCTL))) |= (E1000_TCTL_EN);
    (*((uint32_t *) (start + E1000_TCTL))) |= (0x100); //(E1000_TCTL_CT)
    (*((uint32_t *) (start + E1000_TCTL))) |= (0x40 << 12); //(E1000_TCTL_COLD)
    (*((uint32_t *) (start + E1000_TIPG))) |= (0xA);
    (*((uint32_t *) (start + E1000_TCTL))) |= (E1000_TCTL_EN);
    (*((uint32_t *) (start + E1000_RCTL))) |= E1000_RCTL_EN;
}
/*
 Code Reference : Adopted from code reference of Shashank
 */
void transmit_packet(struct pci_func *e1000)
{
    uint64_t start = (uint64_t) (e1000->start_virtual_address);
    int index = (*((uint32_t *) (start + E1000_TDT)));
    char buf[1512] = "sample data sample data";
    strcpy(trans_buf[index].buf, buf);
    (*(uint32_t *) (&trans_desc[index])) &= (~E1000_TXD_DTYP_D);
    trans_desc[index].lower.flags.length = 100;
    trans_desc[index].lower.flags.cmd = 25;
    (*((uint32_t *) (start + E1000_TDT))) = index + 1;
    while (!((trans_desc[index].upper.fields.status) & 0x1))
        ;
}
/*
 Code Reference : Adopted from code reference of Shashank
 */
void print_status_of_transmission(struct pci_func *e1000)
{
    uint64_t start = (uint64_t) (e1000->start_virtual_address);
    int index = (*((uint32_t *) (start + E1000_TDT)));
    printk("\n TDT=[%d]", index);
}
