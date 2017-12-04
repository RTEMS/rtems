/* Driver Manager Internal locking implementation
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"

void _DRV_Manager_Lock(void)
{
	_API_Mutex_Lock(&drvmgr.lock);
}

void _DRV_Manager_Unlock(void)
{
	_API_Mutex_Unlock(&drvmgr.lock);
}
