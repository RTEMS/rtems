/*
 *  Event Manager
 *
 *  COPYRIGHT (c) 1989-2008.
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
 *  _Event_Seize
 *
 *  This routine attempts to satisfy the requested event condition
 *  for the running thread.
 *
 *  Input parameters:
 *    event_in   - the event condition to satisfy
 *    option_set - acquire event options
 *    ticks      - interval to wait
 *    event_out  - pointer to event set output area
 *
 *  Output parameters: NONE
 *    *event_out - event set output area filled in
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 *    check sync
 */

void _Event_Seize(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
)
{
  Thread_Control                   *executing;
  rtems_event_set                   seized_events;
  rtems_event_set                   pending_events;
  ISR_Level                         level;
  RTEMS_API_Control                *api;
  Thread_blocking_operation_States  sync_state;

  executing = _Thread_Executing;
  executing->Wait.return_code = RTEMS_SUCCESSFUL;

  api = executing->API_Extensions[ THREAD_API_RTEMS ];

  _ISR_Disable( level );
  pending_events = api->pending_events;
  seized_events  = _Event_sets_Get( pending_events, event_in );

  if ( !_Event_sets_Is_empty( seized_events ) &&
       (seized_events == event_in || _Options_Is_any( option_set )) ) {
    api->pending_events =
      _Event_sets_Clear( pending_events, seized_events );
    _ISR_Enable( level );
    *event_out = seized_events;
    return;
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _ISR_Enable( level );
    executing->Wait.return_code = RTEMS_UNSATISFIED;
    *event_out = seized_events;
    return;
  }

  /*
   *  Note what we are waiting for BEFORE we enter the critical section.
   *  The interrupt critical section management code needs this to be
   *  set properly when we are marked as in the event critical section.
   *
   *  NOTE: Since interrupts are disabled, this isn't that much of an
   *        issue but better safe than sorry.
   */
  executing->Wait.option            = (uint32_t) option_set;
  executing->Wait.count             = (uint32_t) event_in;
  executing->Wait.return_argument   = event_out;

  _Event_Sync_state = THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED;

  _ISR_Enable( level );

  if ( ticks ) {
    _Watchdog_Initialize(
      &executing->Timer,
      _Event_Timeout,
      executing->Object.id,
      NULL
    );
    _Watchdog_Insert_ticks( &executing->Timer, ticks );
  }

  _Thread_Set_state( executing, STATES_WAITING_FOR_EVENT );

  _ISR_Disable( level );

  sync_state = _Event_Sync_state;
  _Event_Sync_state = THREAD_BLOCKING_OPERATION_SYNCHRONIZED;
  if ( sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) {
    _ISR_Enable( level );
    return;
  }

  /*
   *  An interrupt completed the thread's blocking request.
   *  The blocking thread was satisfied by an ISR or timed out.
   *
   *  WARNING! Entering with interrupts disabled and returning with interrupts
   *  enabled!
   */
  _Thread_blocking_operation_Cancel( sync_state, executing, level );
}
