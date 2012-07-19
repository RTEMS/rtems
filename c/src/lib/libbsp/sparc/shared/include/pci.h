/*
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997, 1998 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 */

#ifndef RTEMS_PCI_H
#define RTEMS_PCI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/pci.h>

/*
 * Return the number of PCI busses in the system
 */
extern unsigned char BusCountPCI(void);
extern int init_pci(void);

extern int dma_to_pci(unsigned int addr, unsigned int paddr, unsigned int len);
extern int dma_from_pci(unsigned int addr, unsigned int paddr, unsigned int len);
extern void pci_mem_enable(unsigned char bus, unsigned char slot, unsigned char function);
extern void pci_master_enable(unsigned char bus, unsigned char slot, unsigned char function);

/* scan for a specific device */
/* find a particular PCI device
 * (currently, only bus0 is scanned for device/fun0)
 *
 * RETURNS: zero on success, bus/dev/fun in *pbus / *pdev / *pfun
 */
int
BSP_pciFindDevice(unsigned short vendorid, unsigned short deviceid,
                int instance, int *pbus, int *pdev, int *pfun);

#ifdef __cplusplus
}
#endif

#endif /* RTEMS_PCI_H */
