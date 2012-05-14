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

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>
#include <rtems/score/timespec.h>

/*
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */

int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  Watchdog_Interval  ticks;


  /*
   *  Return EINVAL if the delay interval is negative.
   *
   *  NOTE:  This behavior is beyond the POSIX specification.
   *         FSU and GNU/Linux pthreads shares this behavior.
   */
  if ( !_Timespec_Is_valid( rqtp ) )
    rtems_set_errno_and_return_minus_one( EINVAL );

  ticks = _Timespec_To_ticks( rqtp );

  /*
   *  A nanosleep for zero time is implemented as a yield.
   *  This behavior is also beyond the POSIX specification but is
   *  consistent with the RTEMS API and yields desirable behavior.
   */

  if ( !ticks ) {
    _Thread_Disable_dispatch();
      _Scheduler_Yield();
    _Thread_Enable_dispatch();
    if ( rmtp ) {
       rmtp->tv_sec = 0;
       rmtp->tv_nsec = 0;
    }
    return 0;
  }

  /*
   *  Block for the desired amount of time
   */
  _Thread_Disable_dispatch();
    _Thread_Set_state(
      _Thread_Executing,
      STATES_DELAYING | STATES_INTERRUPTIBLE_BY_SIGNAL
    );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
  _Thread_Enable_dispatch();

  /* calculate time remaining */

  if ( rmtp ) {
    ticks -=
      _Thread_Executing->Timer.stop_time - _Thread_Executing->Timer.start_time;

    _Timespec_From_ticks( ticks, rmtp );

    /*
     *  Only when POSIX is enabled, can a sleep be interrupted.
     */
    #if defined(RTEMS_POSIX_API)
        /*
         *  If there is time remaining, then we were interrupted by a signal.
         */
        if ( ticks )
          rtems_set_errno_and_return_minus_one( EINTR );
    #endif
  }

  return 0;
}
