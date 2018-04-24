/* Driver Manager Internal locking implementation
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <drvmgr/drvmgr.h>
#include "drvmgr_internal.h"
#include <rtems/score/sysstate.h>

void _DRV_Manager_Lock( void )
{
  if ( !_System_state_Is_before_initialization( _System_state_Get() ) ) {
    _API_Mutex_Lock( &drvmgr.lock );
  }
}

void _DRV_Manager_Unlock(void)
{
  if ( !_System_state_Is_before_initialization( _System_state_Get() ) ) {
    _API_Mutex_Unlock( &drvmgr.lock );
  }
}
