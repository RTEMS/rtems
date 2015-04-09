/* Driver Manager Initialization
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>

/* Init driver manager - all in one go. Typically called from Init task when
 * user wants to initilize driver manager after startup, otherwise not used.
 */
int drvmgr_init(void)
{
	int level;

	_DRV_Manager_initialization();

	for (level = 1; level <= DRVMGR_LEVEL_MAX; level++)
		_DRV_Manager_init_level(level);

	return 0;
}
