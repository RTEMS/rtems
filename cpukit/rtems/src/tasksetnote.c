/*
 *  RTEMS Task Manager
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*
 * rtems_task_set_note
 *
 *  This directive sets the specified notepad contents to the given
 *  note.
 *
 *  Input parameters:
 *    id      - thread id
 *    notepad - notepad number
 *    note    - note value
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_task_set_note(
  rtems_id id,
  uint32_t notepad,
  uint32_t note
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  RTEMS_API_Control       *api;

  if ( !rtems_configuration_get_notepads_enabled() )
    return RTEMS_NOT_CONFIGURED;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return RTEMS_INVALID_NUMBER;

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, _Thread_Executing->Object.id ) ) {
      api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];
      api->Notepads[ notepad ] = note;
      return RTEMS_SUCCESSFUL;
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      api->Notepads[ notepad ] = note;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_SET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        note
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
