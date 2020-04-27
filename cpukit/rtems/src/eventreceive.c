/**
 * @file
 *
 * @ingroup ClassicEventSet Event Set
 *
 * @brief  Constant used to receive the set of currently pending events in
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_event_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  rtems_status_code sc;

  if ( event_out != NULL ) {
    ISR_lock_Context   lock_context;
    Thread_Control    *executing;
    RTEMS_API_Control *api;
    Event_Control     *event;

    executing = _Thread_Wait_acquire_default_for_executing( &lock_context );
    api = executing->API_Extensions[ THREAD_API_RTEMS ];
    event = &api->Event;

    if ( !_Event_sets_Is_empty( event_in ) ) {
      sc = _Event_Seize(
        event_in,
        option_set,
        ticks,
        event_out,
        executing,
        event,
        THREAD_WAIT_CLASS_EVENT,
        STATES_WAITING_FOR_EVENT,
        &lock_context
      );
    } else {
      *event_out = event->pending_events;
      _Thread_Wait_release_default( executing, &lock_context );
      sc = RTEMS_SUCCESSFUL;
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}
