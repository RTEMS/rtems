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
 *  sta_tsk - Start Task
 */

ER sta_tsk(
  ID   tskid,
  INT  stacd
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  boolean                  status;

  the_thread = _ITRON_Task_Get( tskid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Task_Clarify_get_id_error( tskid ); 

    case OBJECTS_LOCAL:

      if ( !_States_Is_dormant( the_thread->current_state ) )
        _ITRON_return_errorno( E_OBJ );

      status = _Thread_Start(
        the_thread,
        THREAD_START_NUMERIC, /* XXX should be able to say we have no arg */
        the_thread->Start.entry_point,
        0,                    /* XXX supercore forces us to have an arg */
        0                     /* unused */
      );

      /* 
       * This error can not happen entry_point is checked in create. 
         if ( !status )           
           _ITRON_return_errorno(  E_OBJ );
       */

      break;
  }

  _ITRON_return_errorno( E_OK ); 
}




