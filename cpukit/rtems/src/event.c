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
#include <rtems/event.h>
#include <rtems/isr.h>
#include <rtems/object.h>
#include <rtems/options.h>
#include <rtems/states.h>
#include <rtems/thread.h>

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

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return(
        _Event_MP_Send_request_packet(
          EVENT_MP_SEND_REQUEST,
          id,
          event_in
        )
      );
    case OBJECTS_LOCAL:
      _Event_sets_Post( event_in, &the_thread->pending_events );
      _Event_Surrender( the_thread );
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
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
  if ( _Event_sets_Is_empty( event_in ) ) {
    *event_out = _Thread_Executing->pending_events;
    return( RTEMS_SUCCESSFUL );
  }

  _Thread_Disable_dispatch();
  _Event_Seize( event_in, option_set, ticks );
  _Thread_Enable_dispatch();
  *event_out = _Thread_Executing->events_out;
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
 *
 *  Output parameters: NONE
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 *    check sync
 */

void _Event_Seize(
  rtems_event_set event_in,
  rtems_option    option_set,
  rtems_interval  ticks
)
{
  Thread_Control   *executing;
  rtems_event_set   seized_events;
  rtems_event_set   pending_events;
  ISR_Level         level;

  executing = _Thread_Executing;
  executing->Wait.return_code = RTEMS_SUCCESSFUL;

  _ISR_Disable( level );
  pending_events = executing->pending_events;
  seized_events  = _Event_sets_Get( pending_events, event_in );

  if ( !_Event_sets_Is_empty( seized_events ) &&
       (seized_events == event_in || _Options_Is_any( option_set )) ) {
    executing->pending_events =
      _Event_sets_Clear( pending_events, seized_events );
    _ISR_Enable( level );
    executing->events_out = seized_events;
    return;
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _ISR_Enable( level );
    executing->Wait.return_code = RTEMS_UNSATISFIED;
    executing->events_out = seized_events;
    return;
  }

  _Event_Sync = TRUE;
  executing->Wait.option_set            = option_set;
  executing->Wait.Extra.event_condition = event_in;

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
  ISR_Level          level;
  rtems_event_set pending_events;
  rtems_event_set event_condition;
  rtems_event_set seized_events;

  _ISR_Disable( level );
  pending_events  = the_thread->pending_events;
  event_condition = the_thread->Wait.Extra.event_condition;

  seized_events = _Event_sets_Get( pending_events, event_condition );

  if ( !_Event_sets_Is_empty( seized_events ) ) {
    if ( _States_Is_waiting_for_event( the_thread->current_state ) ) {
      if ( seized_events == event_condition ||
                    _Options_Is_any( the_thread->Wait.option_set ) ) {
        the_thread->pending_events =
           _Event_sets_Clear( pending_events, seized_events );
        (rtems_event_set *)the_thread->events_out = seized_events;

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
      if ( seized_events == event_condition ||
                    _Options_Is_any( the_thread->Wait.option_set ) ) {
        the_thread->pending_events =
           _Event_sets_Clear( pending_events,seized_events );
        (rtems_event_set *)the_thread->events_out = seized_events;
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
