/* Find device by device name
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 */

#include <string.h>
#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

static intptr_t dev_name_compare(struct drvmgr_dev *dev, void *arg)
{
	const char *name = arg;

	if (dev->name && (strcmp(dev->name, name) == 0))
		return (intptr_t)dev;
	else
		return 0;
}

/* Get device by device name or bus name */
struct drvmgr_dev *drvmgr_dev_by_name(const char *name)
{
	if (!name)
		return NULL;

	return (struct drvmgr_dev *)
		drvmgr_for_each_dev(dev_name_compare, (void *)name, 0);
}
