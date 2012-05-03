/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
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

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/timer.h>

/*
 * timer_getoverrun
 *
 * The expiration of a timer must increase by one a counter.
 * After the signal handler associated to the timer finishes
 * its execution, _POSIX_Timer_TSR will have to set this counter to 0.
 */
int timer_getoverrun(
  timer_t   timerid
)
{
  int                  overrun;
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;

  ptimer = _POSIX_Timer_Get( timerid, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      overrun = ptimer->overrun;
      ptimer->overrun = 0;
      _Thread_Enable_dispatch();
      return overrun;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
