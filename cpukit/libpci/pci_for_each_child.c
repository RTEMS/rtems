/*  PCI Help function, iterate all PCI device children of PCI bus.
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci/cfg.h>

/* Iterate over all PCI devices on a bus (not child buses) and call func(),
 * iteration is stopped if a non-zero value is returned by func().
 *
 * search options: 0 (no child buses), 1 (depth first), 2 (breadth first)
 */
int pci_for_each_child(
	struct pci_bus *bus,
	int (*func)(struct pci_dev *, void *arg),
	void *arg,
	int search)
{
	struct pci_dev *dev = bus->devs;
	int ret;

	while (dev) {
		ret = func(dev, arg);
		if (ret)
			return ret;
		if (search == SEARCH_DEPTH && (dev->flags & PCI_DEV_BRIDGE)) {
			ret = pci_for_each_child((struct pci_bus *)dev,
							func, arg, search);
			if (ret)
				return ret;
		}
		dev = dev->next;
	}

	return 0;
}
