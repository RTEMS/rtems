/*
 *  Event Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/event.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/tasks.h>

/*PAGE
 *
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
  Thread_Control    *executing;
  rtems_event_set    seized_events;
  rtems_event_set    pending_events;
  ISR_Level          level;
  RTEMS_API_Control  *api;
  Event_Sync_states   sync_state;

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

  _Event_Sync_state = EVENT_SYNC_NOTHING_HAPPENED;

  executing->Wait.option            = (unsigned32) option_set;
  executing->Wait.count             = (unsigned32) event_in;
  executing->Wait.return_argument   = event_out;

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
  _Event_Sync_state = EVENT_SYNC_SYNCHRONIZED;

  switch ( sync_state ) {
    case EVENT_SYNC_SYNCHRONIZED:
      /*
       *  This cannot happen.  It indicates that this routine did not
       *  enter the synchronization states above.
       */
      return;

    case EVENT_SYNC_NOTHING_HAPPENED:
      _ISR_Enable( level );
      return;

    case EVENT_SYNC_TIMEOUT:
      executing->Wait.return_code = RTEMS_TIMEOUT;
      _ISR_Enable( level );
      _Thread_Unblock( executing );
      return;

    case EVENT_SYNC_SATISFIED:
      if ( _Watchdog_Is_active( &executing->Timer ) ) {
        _Watchdog_Deactivate( &executing->Timer );
        _ISR_Enable( level );
        (void) _Watchdog_Remove( &executing->Timer );
      } else
        _ISR_Enable( level );
      _Thread_Unblock( executing );
      return;
  }
}
