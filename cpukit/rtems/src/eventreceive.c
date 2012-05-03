/*
 *  Event Manager
 *
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*
 *  rtems_event_receive
 *
 *  This directive allows a thread to receive a set of events.
 *
 *  Input parameters:
 *    event_in   - input event condition
 *    option_set - options
 *    ticks      - number of ticks to wait (0 means wait forever)
 *    event_out  - pointer to output event set
 *
 *  Output parameters:
 *    event out         - event set
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_event_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  RTEMS_API_Control       *api;

  if ( !event_out )
    return RTEMS_INVALID_ADDRESS;

  api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];

  if ( _Event_sets_Is_empty( event_in ) ) {
    *event_out = api->pending_events;
    return RTEMS_SUCCESSFUL;
  }

  _Thread_Disable_dispatch();
  _Event_Seize( event_in, option_set, ticks, event_out );
  _Thread_Enable_dispatch();
  return( _Thread_Executing->Wait.return_code );
}
