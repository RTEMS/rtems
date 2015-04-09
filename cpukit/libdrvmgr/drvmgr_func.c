/* Driver Manager optional dynamic function interface
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>

/* Get Function from Function ID */
int drvmgr_func_get(void *obj, int funcid, void **func)
{
	int objtype;
	struct drvmgr_func *f;

	if (!obj)
		return DRVMGR_FAIL;
	objtype = *(int *)obj;

	if (objtype == DRVMGR_OBJ_BUS)
		f = ((struct drvmgr_bus *)obj)->funcs;
	else if (objtype == DRVMGR_OBJ_DRV)
		f = ((struct drvmgr_drv *)obj)->funcs;
	else
		return DRVMGR_FAIL;

	if (f == NULL)
		return DRVMGR_FAIL;

	while (f->funcid != DRVMGR_FUNCID_NONE) {
		if (f->funcid == funcid) {
			*func = f->func;
			return DRVMGR_OK;
		}
		f++;
	}

	return DRVMGR_FAIL;
}
