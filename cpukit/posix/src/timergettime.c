/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
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

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/timespec.h>
#include <rtems/posix/timer.h>

/*
 *          - When a timer is initialized, the value of the time in
 *            that moment is stored.
 *          - When this function is called, it returns the difference
 *            between the current time and the initialization time.
 */

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;
  struct timespec      current_time;
  Watchdog_Interval    left;

  if ( !value )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /* Reads the current time */
  _TOD_Get( &current_time );

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:

      /* Calculates the time left before the timer finishes */

      left =
        (ptimer->Timer.start_time + ptimer->Timer.initial) - /* expire */
        _Watchdog_Ticks_since_boot;                          /* now */

      _Timespec_From_ticks( left, &value->it_value );

      value->it_interval  = ptimer->timer_data.it_interval;

      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
