/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>


/*
 *  ext_tsk - Exit Issuing Task
 */

void ext_tsk( void )
{
  _Thread_Disable_dispatch();

  _Thread_Restart( _Thread_Executing, NULL, 0 );
  _Thread_Set_state( _Thread_Executing, STATES_DORMANT );

  _Thread_Enable_dispatch();  
}
