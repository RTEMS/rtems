/**
 * @file
 *
 * @brief  Constant used to receive the set of currently pending events in
 * @ingroup ClassicEventSet Event Set
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threaddispatch.h>

rtems_status_code rtems_event_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  rtems_status_code sc;

  if ( event_out != NULL ) {
    Thread_Control    *executing = _Thread_Get_executing();
    RTEMS_API_Control *api = executing->API_Extensions[ THREAD_API_RTEMS ];
    Event_Control     *event = &api->Event;

    if ( !_Event_sets_Is_empty( event_in ) ) {
      _Thread_Disable_dispatch();
      _Event_Seize(
        event_in,
        option_set,
        ticks,
        event_out,
        executing,
        event,
        &_Event_Sync_state,
        STATES_WAITING_FOR_EVENT
      );
      _Thread_Enable_dispatch();

      sc = executing->Wait.return_code;
    } else {
      *event_out = event->pending_events;
      sc = RTEMS_SUCCESSFUL;
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  return sc;
}
