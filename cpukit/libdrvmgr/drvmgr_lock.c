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
	rtems_semaphore_obtain(drvmgr.lock, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
}

void _DRV_Manager_Unlock(void)
{
	rtems_semaphore_release(drvmgr.lock);
}

int _DRV_Manager_Init_Lock(void)
{
	int rc;

	rc = rtems_semaphore_create(
		rtems_build_name('D', 'R', 'V', 'M'),
		1,
		RTEMS_DEFAULT_ATTRIBUTES,
		0,
		&drvmgr.lock);
	if (rc != RTEMS_SUCCESSFUL)
		return -1;
	return 0;
}
