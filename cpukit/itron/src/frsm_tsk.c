/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
 *  frsm_tsk - Forcibly Resume Suspended Task
 */

ER frsm_tsk(
  ID tskid
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _ITRON_Task_Get( tskid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Task_Clarify_get_id_error( tskid ); 

    case OBJECTS_LOCAL:
      if ( _Thread_Is_executing( the_thread ) )
        _ITRON_return_errorno( E_OBJ );

      if (_States_Is_dormant( the_thread->current_state ))
        _ITRON_return_errorno( E_OBJ );
    
      if ( ! _States_Is_suspended(the_thread->current_state) )
        _ITRON_return_errorno( E_OK );

      _Thread_Resume( the_thread, TRUE );
      break;
  }

 _ITRON_return_errorno( E_OK );

}


