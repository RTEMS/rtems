/**
 *  @file
 *
 *  @brief RTEMS Timer Fire When
 *  @ingroup ClassicTimer
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/timerimpl.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_timer_fire_when(
  rtems_id                            id,
  rtems_time_of_day                  *wall_time,
  rtems_timer_service_routine_entry   routine,
  void                               *user_data
)
{
  Timer_Control       *the_timer;
  Objects_Locations    location;
  rtems_interval       seconds;

  if ( !_TOD.is_set )
    return RTEMS_NOT_DEFINED;

  if ( !_TOD_Validate( wall_time ) )
    return RTEMS_INVALID_CLOCK;

  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  seconds = _TOD_To_seconds( wall_time );
  if ( seconds <= _TOD_Seconds_since_epoch() )
    return RTEMS_INVALID_CLOCK;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      (void) _Watchdog_Remove( &the_timer->Ticker );
      the_timer->the_class = TIMER_TIME_OF_DAY;
      _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
      _Watchdog_Insert_seconds(
         &the_timer->Ticker,
         seconds - _TOD_Seconds_since_epoch()
       );
      _Objects_Put( &the_timer->Object );
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
