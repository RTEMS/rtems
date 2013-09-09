/**
 *  @file
 *
 *  @brief RTEMS Timer Server Fire After
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

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_timer_server_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  Timer_Control        *the_timer;
  Objects_Locations     location;
  ISR_Level             level;
  Timer_server_Control *timer_server = _Timer_server;

  if ( !timer_server )
    return RTEMS_INCORRECT_STATE;

  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

  if ( ticks == 0 )
    return RTEMS_INVALID_NUMBER;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      (void) _Watchdog_Remove( &the_timer->Ticker );

      _ISR_Disable( level );

        /*
         *  Check to see if the watchdog has just been inserted by a
         *  higher priority interrupt.  If so, abandon this insert.
         */

        if ( the_timer->Ticker.state != WATCHDOG_INACTIVE ) {
          _ISR_Enable( level );
          _Objects_Put( &the_timer->Object );
          return RTEMS_SUCCESSFUL;
        }

        /*
         *  OK.  Now we now the timer was not rescheduled by an interrupt
         *  so we can atomically initialize it as in use.
         */

        the_timer->the_class = TIMER_INTERVAL_ON_TASK;
        _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
        the_timer->Ticker.initial = ticks;
      _ISR_Enable( level );

      (*timer_server->schedule_operation)( timer_server, the_timer );

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
