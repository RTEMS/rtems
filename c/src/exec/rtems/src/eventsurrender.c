/*
 *  Event Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

  if ( !_Event_sets_Is_empty( seized_events ) ) {
    if ( _States_Is_waiting_for_event( the_thread->current_state ) ) {
      if ( seized_events == event_condition || _Options_Is_any( option_set ) ) {
        api->pending_events =
           _Event_sets_Clear( pending_events, seized_events );
        *(rtems_event_set *)the_thread->Wait.return_argument = seized_events;
        (rtems_event_set) the_thread->Wait.count = 0; /* FIX 26MAR01 */

        _ISR_Flash( level );

        if ( !_Watchdog_Is_active( &the_thread->Timer ) ) {
          _ISR_Enable( level );
          _Thread_Unblock( the_thread );
        }
        else {
          _Watchdog_Deactivate( &the_thread->Timer );
          _ISR_Enable( level );
          (void) _Watchdog_Remove( &the_thread->Timer );
          _Thread_Unblock( the_thread );
        }
        return;
      }
    }

    switch ( _Event_Sync_state ) {
      case EVENT_SYNC_SYNCHRONIZED:
      case EVENT_SYNC_SATISFIED:
        break;
 
      case EVENT_SYNC_NOTHING_HAPPENED:
      case EVENT_SYNC_TIMEOUT:
        if ( !_Thread_Is_executing( the_thread ) )
          break;

        if ( seized_events == event_condition || _Options_Is_any(option_set) ) {
          api->pending_events = 
               _Event_sets_Clear( pending_events,seized_events );
          *(rtems_event_set *)the_thread->Wait.return_argument = seized_events;
          _Event_Sync_state = EVENT_SYNC_SATISFIED;
        }
        break;
    }
  }
  _ISR_Enable( level );
}
