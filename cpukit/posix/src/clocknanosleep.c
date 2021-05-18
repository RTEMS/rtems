/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Suspends Execution of calling thread until Time elapses
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/timespec.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/posix/posixapi.h>

static Thread_queue_Control _Nanosleep_Pseudo_queue =
  THREAD_QUEUE_INITIALIZER( "Nanosleep" );

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
  Thread_queue_Context queue_context;
  bool                 absolute;
  Thread_Control      *executing;
  int                  eno;

  if ( clock_id != CLOCK_REALTIME && clock_id != CLOCK_MONOTONIC ) {
    return ENOTSUP;
  }

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_set_thread_state(
    &queue_context,
    STATES_WAITING_FOR_TIME | STATES_INTERRUPTIBLE_BY_SIGNAL
  );

  if ( ( flags & TIMER_ABSTIME ) != 0 ) {
    absolute = true;
    rmtp = NULL;
  } else {
    absolute = false;
  }

  if ( clock_id == CLOCK_REALTIME ) {
    _Thread_queue_Context_set_enqueue_timeout_realtime_timespec(
      &queue_context,
      rqtp,
      absolute
    );
  } else {
    _Thread_queue_Context_set_enqueue_timeout_monotonic_timespec(
      &queue_context,
      rqtp,
      absolute
    );
  }

  _Thread_queue_Acquire( &_Nanosleep_Pseudo_queue, &queue_context );
  executing = _Thread_Executing;
  _Thread_queue_Enqueue(
    &_Nanosleep_Pseudo_queue.Queue,
    &_Thread_queue_Operations_FIFO,
    executing,
    &queue_context
  );
  eno = _POSIX_Get_error_after_wait( executing );

  if ( eno == ETIMEDOUT ) {
    eno = 0;
  }

  if ( rmtp != NULL ) {
#if defined( RTEMS_POSIX_API )
    if ( eno == EINTR ) {
      struct timespec actual_end;
      struct timespec planned_end;

      if ( clock_id == CLOCK_REALTIME ) {
        _Timecounter_Nanotime( &actual_end );
      } else {
        _Timecounter_Nanouptime( &actual_end );
      }

      _Watchdog_Ticks_to_timespec(
        executing->Timer.Watchdog.expire,
        &planned_end
      );

      if ( _Timespec_Less_than( &actual_end, &planned_end ) ) {
        _Timespec_Subtract( &actual_end, &planned_end, rmtp );
      } else {
        _Timespec_Set_to_zero( rmtp );
      }
    } else {
      _Timespec_Set_to_zero( rmtp );
    }
#else
    _Assert( eno != EINTR );
    _Timespec_Set_to_zero( rmtp );
#endif
  }

  return eno;
}
