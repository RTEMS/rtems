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
  clockid_t             clock_id,
  uint64_t              ticks,
  struct timespec      *timeout,
  struct timespec      *rmtp,
  Watchdog_Discipline   discipline
)
{
  Thread_queue_Context queue_context;
  struct timespec      stop;
  int                  err;

  err = 0;

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_thread_state(
    &queue_context,
    STATES_WAITING_FOR_TIME | STATES_INTERRUPTIBLE_BY_SIGNAL
  );
  _Thread_queue_Context_set_enqueue_callout(
    &queue_context,
    _Thread_queue_Enqueue_do_nothing
  );

  if ( discipline == WATCHDOG_ABSOLUTE ) {
    _Thread_queue_Context_set_absolute_timeout( &queue_context, ticks );
  } else {
    _Thread_queue_Context_set_relative_timeout( &queue_context, ticks );
  }

  /*
   *  Block for the desired amount of time
   */
  _Thread_queue_Acquire( &_Nanosleep_Pseudo_queue, &queue_context );
  _Thread_queue_Enqueue(
    &_Nanosleep_Pseudo_queue.Queue,
    &_Thread_queue_Operations_FIFO,
    _Thread_Executing,
    &queue_context
  );

  clock_gettime( clock_id, &stop );
  /*
   * If the user wants the time remaining, do the conversion.
   */
  if ( _Timespec_Less_than( &stop, timeout ) ) {
    /*
     *  If there is time remaining, then we were interrupted by a signal.
     */
    err = EINTR;
    if ( rmtp != NULL ) {
      _Timespec_Subtract( &stop, timeout, rmtp );
    }
  } else if ( rmtp != NULL ) {
    /* no time remaining */
    _Timespec_Set_to_zero( rmtp );
  }

  return err;
}

/*
 * A nanosleep for zero time is implemented as a yield.
 * This behavior is also beyond the POSIX specification but is
 * consistent with the RTEMS API and yields desirable behavior.
 */
static inline int nanosleep_yield( struct timespec *rmtp )
{
  /*
   * It is critical to obtain the executing thread after thread dispatching is
   * disabled on SMP configurations.
   */
  Thread_Control  *executing;
  Per_CPU_Control *cpu_self;

  executing = _Thread_Get_executing();
  cpu_self = _Thread_Dispatch_disable();
  _Thread_Yield( executing );
  _Thread_Dispatch_direct( cpu_self );
  if ( rmtp ) {
    rmtp->tv_sec = 0;
    rmtp->tv_nsec = 0;
  }
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
  int err;
  struct timespec now;
  uint64_t ticks;
  Watchdog_Interval relative_interval;

  /*
   *  Return EINVAL if the delay interval is negative.
   *
   *  NOTE:  This behavior is beyond the POSIX specification.
   *         FSU and GNU/Linux pthreads shares this behavior.
   */
  if ( !_Timespec_Is_valid( rqtp ) ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  relative_interval = _Timespec_To_ticks( rqtp );
  if ( relative_interval == 0 )
    return nanosleep_yield( rmtp );

 /* CLOCK_REALTIME can be adjusted during the timeout,
  * so convert to an absolute timeout value and put the
  * thread on the WATCHDOG_ABSOLUTE threadq. */
  err = clock_gettime( CLOCK_REALTIME, &now );
  if ( err != 0 )
    return -1;
  _Timespec_Add_to( &now, rqtp );
  ticks = _Watchdog_Ticks_from_timespec( &now );
  err = nanosleep_helper( CLOCK_REALTIME,
      ticks,
      &now,
      rmtp,
      WATCHDOG_ABSOLUTE
  );
  if ( err != 0 ) {
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
  struct timespec timeout;
  uint64_t ticks;
  Watchdog_Interval relative_interval;
  TOD_Absolute_timeout_conversion_results status;

  if ( !_Timespec_Is_valid( rqtp ) )
    return EINVAL;

  /* get relative ticks of the requested timeout */
  if ( flags & TIMER_ABSTIME ) {
    /* See if absolute time already passed */
    status = _TOD_Absolute_timeout_to_ticks(rqtp, clock_id, &relative_interval);
    if ( status == TOD_ABSOLUTE_TIMEOUT_INVALID )
      return EINVAL;
    if ( status == TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST )
      return 0;
    if ( status == TOD_ABSOLUTE_TIMEOUT_IS_NOW )
      return nanosleep_yield( NULL );
    rmtp = NULL; /* Do not touch rmtp when using absolute time */
    timeout.tv_sec = 0;
    timeout.tv_nsec = 0;
  } else {
    /* prepare to convert relative ticks to absolute timeout */
    err = clock_gettime( clock_id, &timeout );
    if ( err != 0 )
      return EINVAL;
    relative_interval = _Timespec_To_ticks( rqtp );
  }
  if ( relative_interval == 0 )
    return nanosleep_yield( rmtp );

  _Timespec_Add_to( &timeout, rqtp );
  if ( clock_id == CLOCK_REALTIME ) {
    ticks = _Watchdog_Ticks_from_timespec( &timeout );
    err = nanosleep_helper(
        clock_id,
        ticks,
        &timeout,
        rmtp,
        WATCHDOG_ABSOLUTE
    );
  } else if ( clock_id == CLOCK_MONOTONIC ) {
    /* use the WATCHDOG_RELATIVE to ignore changes in wall time */
    err = nanosleep_helper(
        clock_id,
        relative_interval,
        &timeout,
        rmtp,
        WATCHDOG_RELATIVE
    );
  } else {
    err = ENOTSUP;
  }
  return err;
}
