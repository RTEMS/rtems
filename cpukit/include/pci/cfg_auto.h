/* PCI Auto Configuration Library
 *
 * COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __PCI_CFG_AUTO_H__
#define __PCI_CFG_AUTO_H__

#define CFGOPT_NOSETUP_IRQ 0x1 /* Skip IRQ setup */

/* PCI Memory Layout setup, used by the auto-config library in order to
 * determine the addresses of PCI BARs and Buses.
 *
 * All addresses are in PCI address space, the actual address the CPU access
 * may be different, and taken care of elsewhere.
 */
struct pci_auto_setup {
	int options;

	/* PCI prefetchable Memory space (OPTIONAL) */
	uint32_t mem_start;
	uint32_t mem_size; /* 0 = Use MEMIO space for prefetchable mem BARs */

	/* PCI non-prefetchable Memory */
	uint32_t memio_start;
	uint32_t memio_size;

	/* PCI I/O space (OPTIONAL) */
	uint32_t io_start;
	uint32_t io_size; /* 0 = No I/O space */

	/* Get System IRQ connected to a PCI line of a PCI device on bus0.
	 * The return IRQ value zero equals no IRQ (IRQ disabled).
	 */
	uint8_t (*irq_map)(pci_dev_t dev, int irq_pin);

	/* IRQ Bridge routing. Returns the interrupt pin (0..3 = A..D) that
	 * a device is connected to on parent bus.
	 */
	int (*irq_route)(pci_dev_t dev, int irq_pin);
};

/* Do PCI initialization: Enumrate buses, scan buses for devices, assign
 * I/O MEM and MEMIO resources, assign IRQ and so on.
 */
extern int pci_config_auto(void);

/* Register a configuration for the auto library (struct pci_auto_setup *) */
extern void pci_config_auto_register(void *config);

/* PCI memory map */
extern struct pci_auto_setup pci_auto_cfg;

#endif
