/**
 *  @file
 *
 *  @brief RTEMS Get Task Node
 *  @ingroup ClassicRTEMS
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>

rtems_status_code rtems_task_get_note(
  rtems_id    id,
  uint32_t    notepad,
  uint32_t   *note
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  RTEMS_API_Control       *api;
  Thread_Control          *executing;

  if ( !rtems_configuration_get_notepads_enabled() )
    return RTEMS_NOT_CONFIGURED;

  if ( !note )
    return RTEMS_INVALID_ADDRESS;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return RTEMS_INVALID_NUMBER;

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  executing = _Thread_Get_executing();
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, executing->Object.id ) ) {
      api = executing->API_Extensions[ THREAD_API_RTEMS ];
      *note = api->Notepads[ notepad ];
      return RTEMS_SUCCESSFUL;
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      *note = api->Notepads[ notepad ];
      _Objects_Put( &the_thread->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      executing->Wait.return_argument = note;

      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_GET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
