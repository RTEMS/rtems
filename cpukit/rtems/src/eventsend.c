/**
 *  @file
 *
 *  @brief Sends an Event Set to the Target Task
 *  @ingroup ClassicEvent
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

#include <rtems/rtems/event.h>
#include <rtems/rtems/tasks.h>

rtems_status_code rtems_event_send(
  rtems_id        id,
  rtems_event_set event_in
)
{
  rtems_status_code  sc;
  Thread_Control    *thread;
  Objects_Locations  location;
  RTEMS_API_Control *api;

  thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_LOCAL:
      api = thread->API_Extensions[ THREAD_API_RTEMS ];
      _Event_Surrender(
        thread,
        event_in,
        &api->Event,
        &_Event_Sync_state,
        STATES_WAITING_FOR_EVENT
      );
      _Objects_Put( &thread->Object );
      sc = RTEMS_SUCCESSFUL;
      break;
#ifdef RTEMS_MULTIPROCESSING
    case OBJECTS_REMOTE:
      sc = _Event_MP_Send_request_packet(
        EVENT_MP_SEND_REQUEST,
        id,
        event_in
      );
      break;
#endif
    default:
      sc = RTEMS_INVALID_ID;
      break;
  }

  return sc;
}
