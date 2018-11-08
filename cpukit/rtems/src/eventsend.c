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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_event_send(
  rtems_id        id,
  rtems_event_set event_in
)
{
  Thread_Control    *the_thread;
  RTEMS_API_Control *api;
  ISR_lock_Context   lock_context;

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Event_MP_Send( id, event_in );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  return _Event_Surrender(
    the_thread,
    event_in,
    &api->Event,
    THREAD_WAIT_CLASS_EVENT,
    &lock_context
  );
}
