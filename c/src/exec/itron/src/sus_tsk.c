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
 *  Task-Dependent Synchronization Functions
 */

/*
 *  sus_tsk - Suspend Other Task
 */

ER sus_tsk(
  ID tskid
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _ITRON_Task_Get( tskid, &location );
  if (!the_thread)
    _ITRON_return_errorno( _ITRON_Task_Clarify_get_id_error( tskid ) );

  if ( the_thread == _Thread_Executing )
    _ITRON_return_errorno( E_OBJ );

  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      _ITRON_return_errorno( _ITRON_Task_Clarify_get_id_error( tskid ) ); 

    case OBJECTS_LOCAL:
      _Thread_Suspend( the_thread );
      _Thread_Enable_dispatch();
      return E_OK;
  }

  return E_OBJ;           /* XXX - Should never get here */
}







