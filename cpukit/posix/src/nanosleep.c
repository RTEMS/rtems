/**
 * @file
 *
 * @brief Suspends Execution of calling thread until Time elapses
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 * 
 *  Copyright (c) 2016. Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>

#include <rtems/seterr.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/timespec.h>
#include <rtems/score/watchdogimpl.h>

static Thread_queue_Control _Nanosleep_Pseudo_queue =
  THREAD_QUEUE_INITIALIZER( "Nanosleep" );

static inline int nanosleep_helper(
  const struct timespec  *rqtp,
  struct timespec        *rmtp,
  Watchdog_Discipline     discipline
)
{
  /*
   * It is critical to obtain the executing thread after thread dispatching is
   * disabled on SMP configurations.
   */
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  Watchdog_Interval  ticks;
  Watchdog_Interval  start;
  Watchdog_Interval  elapsed;


  /*
   *  Return EINVAL if the delay interval is negative.
   *
   *  NOTE:  This behavior is beyond the POSIX specification.
   *         FSU and GNU/Linux pthreads shares this behavior.
   */
  if ( !_Timespec_Is_valid( rqtp ) )
    return EINVAL;

  /*
   * Convert the timespec delay into the appropriate number of clock ticks.
   */
  ticks = _Timespec_To_ticks( rqtp );

  executing = _Thread_Get_executing();

  /*
   *  A nanosleep for zero time is implemented as a yield.
   *  This behavior is also beyond the POSIX specification but is
   *  consistent with the RTEMS API and yields desirable behavior.
   */
  if ( !ticks ) {
    cpu_self = _Thread_Dispatch_disable();
      _Thread_Yield( executing );
    _Thread_Dispatch_enable( cpu_self );
    if ( rmtp ) {
       rmtp->tv_sec = 0;
       rmtp->tv_nsec = 0;
    }
    return 0;
  }

  start = _Watchdog_Ticks_since_boot;

  /*
   *  Block for the desired amount of time
   */
  _Thread_queue_Enqueue(
    &_Nanosleep_Pseudo_queue,
    &_Thread_queue_Operations_FIFO,
    executing,
    STATES_DELAYING | STATES_INTERRUPTIBLE_BY_SIGNAL,
    ticks,
    discipline,
    1
  );

  /*
   * Calculate the time that passed while we were sleeping and how
   * much remains from what we requested.
   */
  elapsed = _Watchdog_Ticks_since_boot - start;
  if ( elapsed >= ticks )
    ticks = 0;
  else
    ticks -= elapsed;

  /*
   * If the user wants the time remaining, do the conversion.
   */
  if ( rmtp && discipline == WATCHDOG_RELATIVE ) {
    _Timespec_From_ticks( ticks, rmtp );
  }

  /*
   *  Only when POSIX is enabled, can a sleep be interrupted.
   */
  #if defined(RTEMS_POSIX_API)
    /*
     *  If there is time remaining, then we were interrupted by a signal.
     */
    if ( ticks )
      return EINTR;
  #endif

  return 0;
}
/*
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */
int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  int err = nanosleep_helper(rqtp, rmtp, WATCHDOG_RELATIVE);
  if (err) {
    rtems_set_errno_and_return_minus_one( err );
  }
  return 0;
}

/*
 * High Resolution Sleep with Specifiable Clock, IEEE Std 1003.1, 2001
 */
int clock_nanosleep(
  clockid_t               clock_id,
  int                     flags,
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  int err = 0;
  if ( clock_id == CLOCK_REALTIME || clock_id == CLOCK_MONOTONIC ) {
    if ( flags & TIMER_ABSTIME ) {
      err = nanosleep_helper(rqtp, rmtp, WATCHDOG_ABSOLUTE);
    } else {
      err = nanosleep_helper(rqtp, rmtp, WATCHDOG_RELATIVE);
    }
  } else {
    err = ENOTSUP;
  }
  return err;
}
