/*  Device allocator helper used by PCI Auto/Read Configuration Library
 *
 *  COPYRIGHT (c) 2010 Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <rtems/bspIo.h>

#include <pci.h>
#include <pci/cfg.h>

#include "pci_internal.h"

struct pci_dev *pci_dev_create(int isbus)
{
	void *ptr;
	int size;

	if (isbus)
		size = sizeof(struct pci_bus);
	else
		size = sizeof(struct pci_dev);

	ptr = malloc(size);
	if (!ptr)
		rtems_fatal_error_occurred(RTEMS_NO_MEMORY);
	memset(ptr, 0, size);
	return ptr;
}
