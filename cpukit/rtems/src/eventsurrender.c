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
 *  _Event_Surrender
 *
 *  This routines remove a thread from the specified threadq.
 *
 *  Input parameters:
 *    the_thread - pointer to thread to be dequeued
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    before flash
 *    after flash
 *    check sync
 */

void _Event_Surrender(
  Thread_Control *the_thread
)
{
  ISR_Level           level;
  rtems_event_set     pending_events;
  rtems_event_set     event_condition;
  rtems_event_set     seized_events;
  rtems_option        option_set;
  RTEMS_API_Control  *api;

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];

  option_set = (rtems_option) the_thread->Wait.option;

  _ISR_Disable( level );
  pending_events  = api->pending_events;
  event_condition = (rtems_event_set) the_thread->Wait.count;

  seized_events = _Event_sets_Get( pending_events, event_condition );

  /*
   *  No events were seized in this operation
   */
  if ( _Event_sets_Is_empty( seized_events ) ) {
    _ISR_Enable( level );
    return;
  }

  /*
   *  If we are in an ISR and sending to the current thread, then
   *  we have a critical section issue to deal with.
   */
  if ( _ISR_Is_in_progress() &&
       _Thread_Is_executing( the_thread ) &&
       ((_Event_Sync_state == THREAD_BLOCKING_OPERATION_TIMEOUT) ||
        (_Event_Sync_state == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED)) ) {
    if ( seized_events == event_condition || _Options_Is_any(option_set) ) {
      api->pending_events = _Event_sets_Clear( pending_events,seized_events );
      the_thread->Wait.count = 0;
      *(rtems_event_set *)the_thread->Wait.return_argument = seized_events;
      _Event_Sync_state = THREAD_BLOCKING_OPERATION_SATISFIED;
    }
    _ISR_Enable( level );
    return;
  }

  /*
   *  Otherwise, this is a normal send to another thread
   */
  if ( _States_Is_waiting_for_event( the_thread->current_state ) ) {
    if ( seized_events == event_condition || _Options_Is_any( option_set ) ) {
      api->pending_events = _Event_sets_Clear( pending_events, seized_events );
      the_thread->Wait.count = 0;
      *(rtems_event_set *)the_thread->Wait.return_argument = seized_events;

      _ISR_Flash( level );

      if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
        _ISR_Enable( level );
        _Thread_Unblock( the_thread );
      } else {
        _Watchdog_Deactivate( &the_thread->Timer );
        _ISR_Enable( level );
        (void) _Watchdog_Remove( &the_thread->Timer );
        _Thread_Unblock( the_thread );
      }
      return;
    }
  }
  _ISR_Enable( level );
}
