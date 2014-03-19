#ifndef _PCI_H
#define _PCI_H

#include <defs.h>

struct pci_func
{
    uint32_t vendor_id;
    uint32_t device_id;
    uint32_t bus_num;
    uint32_t slot_num;
    uint32_t mmio_reg;
    uint32_t interrupt_reg;
    uint32_t mmio_reg_addr;
    uint32_t mmio_reg_size;
    uint32_t irq_line;
    uint64_t start_virtual_address;
};

void pci_init(void);

extern struct pci_func pci_e1000, pci_ahci;
#endif
