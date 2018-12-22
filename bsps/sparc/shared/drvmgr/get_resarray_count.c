/*  Common driver configuration routines.
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <grlib/bspcommon.h>

int get_resarray_count(struct drvmgr_bus_res **array)
{
	int i = 0;
	while (array[i] != NULL)
		i++;
	return i;
}
