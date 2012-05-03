/*
 *  Timer Manager - rtems_timer_reset directive
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
 *  rtems_timer_reset
 *
 *  This directive allows a thread to reset a timer.
 *
 *  Input parameters:
 *    id - timer id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_timer_reset(
  rtems_id id
)
{
  Timer_Control     *the_timer;
  Objects_Locations  location;
  rtems_status_code  status = RTEMS_SUCCESSFUL;

  the_timer = _Timer_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      if ( the_timer->the_class == TIMER_INTERVAL ) {
        _Watchdog_Remove( &the_timer->Ticker );
        _Watchdog_Insert( &_Watchdog_Ticks_chain, &the_timer->Ticker );
      } else if ( the_timer->the_class == TIMER_INTERVAL_ON_TASK ) {
        Timer_server_Control *timer_server = _Timer_server;

        /*
         *  There is no way for a timer to have this class unless
         *  it was scheduled as a server fire.  That requires that
         *  the Timer Server be initiated.  So this error cannot
         *  occur unless something is internally wrong.
         */
        #if defined(RTEMS_DEBUG)
          if ( !timer_server ) {
            _Thread_Enable_dispatch();
            return RTEMS_INCORRECT_STATE;
          }
        #endif
        _Watchdog_Remove( &the_timer->Ticker );
        (*timer_server->schedule_operation)( timer_server, the_timer );
      } else {
        /*
         *  Must be dormant or time of day timer (e.g. TIMER_DORMANT,
         *  TIMER_TIME_OF_DAY, or TIMER_TIME_OF_DAY_ON_TASK).  We
         *  can only reset active interval timers.
         */
        status = RTEMS_NOT_DEFINED;
      }
      _Thread_Enable_dispatch();
      return status;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
