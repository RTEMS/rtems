/*  PCI Help function, iterate all PCI devices.
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <pci/cfg.h>

int pci_for_each_dev(
	int (*func)(struct pci_dev *, void *arg),
	void *arg)
{
	return pci_for_each_child(&pci_hb, func, arg, SEARCH_DEPTH);
}
