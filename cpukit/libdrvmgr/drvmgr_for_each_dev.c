/* Iterate over device tree topology, breadth or depth-first
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>
#include <drvmgr/drvmgr.h>
#include <drvmgr/drvmgr_list.h>
#include "drvmgr_internal.h"

/* Traverse device tree breadth-first. Supports up to 31 buses */
static intptr_t drvmgr_for_each_dev_breadth(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	int ret = 0, i, pos;
	struct drvmgr_bus *bus, *buses[32];
	struct drvmgr_dev *dev;

	pos = 0;
	memset(&buses[0], 0, sizeof(buses));
	buses[pos++] = drvmgr.root_dev.bus; 	/* Get root bus */

	for (i = 0, bus = buses[0]; buses[i]; i++, bus = buses[i]) {
		dev = bus->children;
		while (dev) {
			ret = func(dev, arg);
			if (ret != 0)
				break;
			if (dev->bus && pos < 31)
				buses[pos++] = dev->bus;

			dev = dev->next_in_bus;
		}
	}

	return ret;
}

/* Traverse device tree depth-first. */
static intptr_t drvmgr_for_each_dev_depth(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	int ret = 0;
	struct drvmgr_dev *dev;

	/* Get first device */
	dev = drvmgr.root_dev.bus->children;

	while (dev) {
		ret = func(dev, arg);
		if (ret != 0)
			break;
		if (dev->bus && dev->bus->children) {
			dev = dev->bus->children;
		} else {
next_dev:
			if (dev->next_in_bus == NULL) {
				/* Step up one level... back to parent bus */
				dev = dev->parent->dev;
				if (dev == &drvmgr.root_dev)
					break;
				goto next_dev;
			} else {
				dev = dev->next_in_bus;
			}
		}
	}

	return ret;
}

/* Traverse device tree depth-first or breadth-first */
intptr_t drvmgr_for_each_dev(
	intptr_t (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg,
	int options
	)
{
	intptr_t ret;

	DRVMGR_LOCK_READ();

	/* Get Root Device */
	if (drvmgr.root_dev.bus->children != NULL) {
		if (options & DRVMGR_FED_BF)
			ret = drvmgr_for_each_dev_breadth(func, arg);
		else
			ret = drvmgr_for_each_dev_depth(func, arg);
	} else
		ret = 0;

	DRVMGR_UNLOCK();

	return ret;
}
