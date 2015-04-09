/* Iterate over one list of devices used internally by driver manager
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>
#include <drvmgr/drvmgr_list.h>
#include "drvmgr_internal.h"

int drvmgr_for_each_listdev(
	struct drvmgr_list *devlist,
	unsigned int state_set_mask,
	unsigned int state_clr_mask,
	int (*func)(struct drvmgr_dev *dev, void *arg),
	void *arg
	)
{
	struct drvmgr_dev *dev;
	int ret = 0;

	DRVMGR_LOCK_READ();

	/* Get First Device */
	dev = DEV_LIST_HEAD(devlist);
	while (dev) {
		if (((state_set_mask != 0) && ((dev->state & state_set_mask) == state_set_mask)) ||
		    ((state_clr_mask != 0) && ((dev->state & state_clr_mask) == 0)) ||
		    ((state_set_mask == 0) && (state_clr_mask == 0))) {
			ret = func(dev, arg);
			if (ret != 0)
				break;
		}
		dev = dev->next;
	}

	DRVMGR_UNLOCK();

	return ret;
}
