/*  PCI IRQ Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>
#include <pci/access.h>
#include <pci/irq.h>

int pci_dev_irq(pci_dev_t dev)
{
	uint8_t irq_line;
	pci_cfg_r8(dev, PCIR_INTLINE, &irq_line);
	return irq_line;
}
