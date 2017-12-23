#ifndef __GRPCI_H__
#define __GRPCI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Register the GRPCI driver to the Driver Manager */
extern void grpci_register_drv(void);

/* Transfer data using GRPCI DMA unit from AMBA to PCI space. Blocks until
 * operation completes.
 */
int grpci_dma_to_pci(
	unsigned int ahb_addr,
	unsigned int pci_addr,
	unsigned int len);

/* Transfer data using GRPCI DMA unit from PCI to AMBA space. Blocks until
 * operation completes.
 */
int grpci_dma_from_pci(
	unsigned int ahb_addr,
	unsigned int pci_addr,
	unsigned int len);

#ifdef __cplusplus
}
#endif

#endif
