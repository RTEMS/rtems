/*  PCI Help function, Find a PCI device by BUS|SLOT|FUNCTION
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>
#include <pci/cfg.h>

static int compare_dev_pcidev(struct pci_dev *dev, void *arg)
{
	pci_dev_t pcidev = (unsigned)arg;

	if (dev->busdevfun == pcidev)
		return (int)dev;
	else
		return 0;
}

/* Get a Device in PCI device tree located in RAM by PCI BUS|SLOT|FUNCTION */
int pci_get_dev(pci_dev_t pcidev, struct pci_dev **ppdev)
{
	int result;

	result = pci_for_each_dev(compare_dev_pcidev, (void *)(unsigned)pcidev);
	if (ppdev)
		*ppdev = (struct pci_dev *)result;
	if (result == 0)
		return -1;
	else
		return 0;
}
