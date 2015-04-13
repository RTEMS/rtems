/* Find driver by driver-ID
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

/* Get driver from driver name */
struct drvmgr_drv *drvmgr_drv_by_id(uint64_t id)
{
	struct drvmgr *mgr = &drvmgr;
	struct drvmgr_drv *drv = NULL;

	/* NOTE: No locking is needed here since Driver list is supposed to be
	 *       initialized once during startup, we treat it as a static
	 *       read-only list
	 */

	drv = DRV_LIST_HEAD(&mgr->drivers);
	while (drv) {
		if (drv->drv_id == id)
			break;
		drv = drv->next;
	}

	return drv;
}
