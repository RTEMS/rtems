/* Driver Manager optional dynamic function interface
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>

/* Lookup function from function ID and call it using given arguments */
int drvmgr_func_call(void *obj, int funcid, void *a, void *b, void *c, void *d)
{
	int (*func)(void *arg1, void *arg2, void *arg3, void *arg4) = NULL;

	if (drvmgr_func_get(obj, funcid, (void *)&func) != DRVMGR_OK)
		return DRVMGR_FAIL;
	return func(a, b, c, d);
}
