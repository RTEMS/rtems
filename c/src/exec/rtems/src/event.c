/*
 *  Event Manager
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
 *  rtems_event_send
 *
 *  This directive allows a thread send an event set to another thread.
 *
 *  Input parameters:
 *    id    - thread id
 *    event - event set
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_event_send(
  Objects_Id         id,
  rtems_event_set event_in
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  RTEMS_API_Control       *api;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;
    case OBJECTS_REMOTE:
      return(
        _Event_MP_Send_request_packet(
          EVENT_MP_SEND_REQUEST,
          id,
          event_in
        )
      );
    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      _Event_sets_Post( event_in, &api->pending_events );
      _Event_Surrender( the_thread );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}

/*PAGE
 *
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

  _Event_Sync = TRUE;
  executing->Wait.option            = (unsigned32) option_set;
  executing->Wait.count             = (unsigned32) event_in;
  executing->Wait.return_argument   = event_out;

  _ISR_Enable( level );
  _Thread_Set_state( executing, STATES_WAITING_FOR_EVENT );

  if ( ticks ) {
    _Watchdog_Initialize(
      &executing->Timer,
      _Event_Timeout,
      executing->Object.id,
      NULL
    );
    _Watchdog_Insert_ticks(
      &executing->Timer,
      ticks,
      WATCHDOG_NO_ACTIVATE
    );
  }

  _ISR_Disable( level );
  if ( _Event_Sync == TRUE ) {
    _Event_Sync = FALSE;
    if ( ticks )
      _Watchdog_Activate( &executing->Timer );
    _ISR_Enable( level );
    return;
  }
  _ISR_Enable( level );
  (void) _Watchdog_Remove( &executing->Timer );
  _Thread_Unblock( executing );
  return;
}

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
    else if ( _Thread_Is_executing( the_thread ) && _Event_Sync == TRUE ) {
      if ( seized_events == event_condition || _Options_Is_any( option_set ) ) {
        api->pending_events = _Event_sets_Clear( pending_events,seized_events );
        *(rtems_event_set *)the_thread->Wait.return_argument = seized_events;
        _Event_Sync = FALSE;
      }
    }
  }
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Event_Timeout
 *
 *  This routine processes a thread which timeouts while waiting to
 *  receive an event_set. It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Event_Timeout(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control *the_thread;
  Objects_Locations      location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      the_thread->Wait.return_code = RTEMS_TIMEOUT;
      _Thread_Unblock( the_thread );
      _Thread_Unnest_dispatch();
      break;
  }
}
