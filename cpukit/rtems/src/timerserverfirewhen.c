/*
 *  Timer Manager - rtems_timer_server fire_when directive
 *
 *
 *  COPYRIGHT (c) 1989-2002.
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
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*PAGE
 *
 *  rtems_timer_server_fire_when
 *
 *  This directive allows a thread to start a timer which will by 
 *  executed by the Timer Server when it fires.
 *
 *  Input parameters:
 *    id        - timer id
 *    wall_time - time of day to fire timer
 *    routine   - routine to schedule
 *    user_data - passed as argument to routine when it is fired
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_timer_server_fire_when(
  Objects_Id                         id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  Timer_Control        *the_timer;
  Objects_Locations     location;
  rtems_interval        seconds;
  extern Chain_Control  _Timer_Seconds_chain;

  if ( !_Timer_Server )
    return RTEMS_INCORRECT_STATE;

  if ( !_TOD_Is_set )
    return RTEMS_NOT_DEFINED;

  if ( !_TOD_Validate( wall_time ) )
    return RTEMS_INVALID_CLOCK;

  seconds = _TOD_To_seconds( wall_time );
  if ( seconds <= _TOD_Seconds_since_epoch )
    return RTEMS_INVALID_CLOCK;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:            /* should never return this */
      return RTEMS_INTERNAL_ERROR;

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      (void) _Watchdog_Remove( &the_timer->Ticker );
      the_timer->the_class = TIMER_TIME_OF_DAY_ON_TASK;
      _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
      the_timer->Ticker.initial = seconds - _TOD_Seconds_since_epoch;

      _Timer_Server_stop_seconds_timer();
      _Timer_Server_process_seconds_chain();
      _Watchdog_Insert( &_Timer_Seconds_chain, &the_timer->Ticker );
       _Timer_Server_reset_seconds_timer();

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
