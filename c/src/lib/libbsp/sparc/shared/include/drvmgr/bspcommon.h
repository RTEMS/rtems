/*  Common BSP/driver configuration routines.
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __BSPCOMMON_H__
#define __BSPCOMMON_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Count driver resource array length. Array must be terminated with a NULL */
int get_resarray_count(struct drvmgr_bus_res **array);

#ifdef __cplusplus
}
#endif

#endif
