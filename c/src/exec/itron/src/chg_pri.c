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
 *  chg_pri - Change Task Priority
 */

ER chg_pri(
  ID  tskid,
  PRI tskpri
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  Priority_Control         new_priority;  

  the_thread = _ITRON_Task_Get( tskid, &location );
  if (!the_thread)
    _ITRON_return_errorno( _ITRON_Task_Clarify_get_id_error( tskid ) );

  if (_States_Is_dormant( the_thread->current_state ))
    return -1;

  if (( tskpri <= 0 ) || ( tskpri >= 256 ))
    _ITRON_return_errorno( E_PAR );

  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      _ITRON_return_errorno( _ITRON_Task_Clarify_get_id_error( tskid )); 

    case OBJECTS_LOCAL:
      new_priority = _ITRON_Task_Priority_to_Core( tskpri );
      the_thread->real_priority = new_priority;

      /*
       * XXX This is from the rtems side and I'm not sure what this is for.
       * XXX Is this check right or should change priority be called 
       *     regardless?
       */

      if ( the_thread->resource_count == 0 ||
           the_thread->current_priority > new_priority )
        _Thread_Change_priority( the_thread, new_priority, FALSE );

      _Thread_Enable_dispatch();
      return E_OK;
  }

  return E_OBJ;  /* XXX - Should never get here */
}


