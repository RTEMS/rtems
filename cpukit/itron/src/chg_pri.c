/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Task_Clarify_get_id_error( tskid ); 

    case OBJECTS_LOCAL:
      if (_States_Is_dormant( the_thread->current_state ))
        _ITRON_return_errorno( E_OBJ );

      if (( tskpri <= 0 ) || ( tskpri >= 256 ))
        _ITRON_return_errorno( E_PAR );

      new_priority = _ITRON_Task_Priority_to_Core( tskpri );
      the_thread->real_priority = new_priority;

      /*
       * The priority should not be changed until later if priority
       * inheratance has occured.
       */

      if ( the_thread->resource_count == 0 ||
           the_thread->current_priority > new_priority )
        _Thread_Change_priority( the_thread, new_priority, FALSE );

      break;
  }

  _ITRON_return_errorno( E_OK );
}


