/*  PCI Help function, Find a PCI device by VENDOR/DEVICE ID
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci.h>
#include <pci/cfg.h>

struct compare_info {
	int index;
	uint16_t vendor;
	uint16_t device;
};

static int compare_dev_id(struct pci_dev *dev, void *arg)
{
	struct compare_info *info = arg;

	if ((dev->vendor != info->vendor) || (dev->device != info->device))
		return 0;
	if (info->index-- == 0)
		return (int)dev;
	else
		return 0;
}

/* Find a Device in PCI device tree located in RAM */
int pci_find_dev(uint16_t ven, uint16_t dev, int index, struct pci_dev **ppdev)
{
	struct compare_info info;
	int result;

	info.index = index;
	info.vendor = ven;
	info.device = dev;

	result = pci_for_each_dev(compare_dev_id, &info);
	if (ppdev)
		*ppdev = (struct pci_dev *)result;
	if (result == 0)
		return -1;
	else
		return 0;
}
