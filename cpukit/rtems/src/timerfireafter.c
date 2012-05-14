/*
 *  Timer Manager - rtems_timer_fire_after directive
 *
 *
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
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  rtems_timer_fire_after
 *
 *  This directive allows a thread to start a timer.
 *
 *  Input parameters:
 *    id        - timer id
 *    ticks     - interval until routine is fired
 *    routine   - routine to schedule
 *    user_data - passed as argument to routine when it is fired
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_timer_fire_after(
  rtems_id                           id,
  rtems_interval                     ticks,
  rtems_timer_service_routine_entry  routine,
  void                              *user_data
)
{
  Timer_Control      *the_timer;
  Objects_Locations   location;
  ISR_Level           level;

  if ( ticks == 0 )
    return RTEMS_INVALID_NUMBER;

  if ( !routine )
    return RTEMS_INVALID_ADDRESS;

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
          _Thread_Enable_dispatch();
          return RTEMS_SUCCESSFUL;
        }

        /*
         *  OK.  Now we now the timer was not rescheduled by an interrupt
         *  so we can atomically initialize it as in use.
         */

        the_timer->the_class = TIMER_INTERVAL;
        _Watchdog_Initialize( &the_timer->Ticker, routine, id, user_data );
      _ISR_Enable( level );


      _Watchdog_Insert_ticks( &the_timer->Ticker, ticks );
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
