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
 *  del_tsk - Delete Task
 */

ER del_tsk(
  ID tskid
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  ER                       result;

  /* XXX - Fix Documentation and error checking for this error on self */

  the_thread = _ITRON_Task_Get( tskid, &location );
  _Thread_Disable_dispatch();

  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      _Thread_Enable_dispatch();  
      return _ITRON_Task_Clarify_get_id_error( tskid );

    case OBJECTS_LOCAL:
      result = _ITRON_Delete_task( the_thread );
  }

  _Thread_Enable_dispatch();
  return E_OK;
}

