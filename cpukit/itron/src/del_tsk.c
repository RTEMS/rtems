/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/apimutex.h>
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
  Objects_Information     *the_information;
  ER                       result = E_OK; /* to avoid warning */

  _RTEMS_Lock_allocator();
  the_thread = _ITRON_Task_Get( tskid, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      _RTEMS_Unlock_allocator();
      return _ITRON_Task_Clarify_get_id_error( tskid );

    case OBJECTS_LOCAL:

      if ( _Thread_Is_executing( the_thread ) ) {
        _RTEMS_Unlock_allocator();
        _ITRON_return_errorno( E_OBJ );
      }

      if ( !_States_Is_dormant( the_thread->current_state ) ) {
        _RTEMS_Unlock_allocator();
        _ITRON_return_errorno( E_OBJ );
      }

      the_information = _Objects_Get_information_id( the_thread->Object.id );
      _Thread_Close( the_information, the_thread );

      _ITRON_Task_Free( the_thread );

      _RTEMS_Unlock_allocator();
      result = E_OK;
      break;
  }

  _ITRON_return_errorno( result );
}
