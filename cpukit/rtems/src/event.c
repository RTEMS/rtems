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
