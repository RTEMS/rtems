/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Add_timeout_ticks(), _Thread_queue_Add_timeout_timespec(),
 *   _Thread_queue_Add_timeout_monotonic_timespec(), and
 *   _Thread_queue_Add_timeout_realtime_timespec().
 */

/*
 * Copyright (c) 2016, 2017 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_queue_Add_timeout_ticks(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  Watchdog_Interval ticks;

  ticks = queue_context->Timeout.ticks;

  if ( ticks != WATCHDOG_NO_TIMEOUT ) {
    _Thread_Add_timeout_ticks(
      the_thread,
      cpu_self,
      queue_context->Timeout.ticks
    );
  }
}

static void _Thread_queue_Add_timeout_timespec(
  Thread_queue_Queue    *queue,
  Thread_Control        *the_thread,
  Per_CPU_Control       *cpu_self,
  Thread_queue_Context  *queue_context,
  Watchdog_Header       *header,
  const struct timespec *now
)
{
  const struct timespec *abstime;

  abstime = queue_context->Timeout.arg;

  if ( _Watchdog_Is_valid_timespec( abstime ) ) {
    uint64_t expire;

    if ( abstime->tv_sec < 0 ) {
      expire = 0;
    } else if ( _Watchdog_Is_far_future_timespec( abstime ) ) {
      expire = WATCHDOG_MAXIMUM_TICKS;
    } else {
      expire = _Watchdog_Ticks_from_timespec( abstime );
    }

    if ( expire > _Watchdog_Ticks_from_timespec( now ) ) {
      ISR_lock_Context lock_context;

      _ISR_lock_ISR_disable_and_acquire(
        &the_thread->Timer.Lock,
        &lock_context
      );

      the_thread->Timer.header = header;
      the_thread->Timer.Watchdog.routine = _Thread_Timeout;
      _Watchdog_Per_CPU_insert(
        &the_thread->Timer.Watchdog,
        cpu_self,
        header,
        expire
      );

      _ISR_lock_Release_and_ISR_enable(
        &the_thread->Timer.Lock,
        &lock_context
      );
    } else {
      _Thread_Continue( the_thread, STATUS_TIMEOUT );
    }
  } else {
    _Thread_Continue( the_thread, STATUS_INVALID_NUMBER );
  }
}

void _Thread_queue_Add_timeout_monotonic_timespec(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  struct timespec now;

  _Timecounter_Getnanouptime( &now );
  _Thread_queue_Add_timeout_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context,
    &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_MONOTONIC ],
    &now
  );
}

void _Thread_queue_Add_timeout_realtime_timespec(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  struct timespec now;

  _Timecounter_Getnanotime( &now );
  _Thread_queue_Add_timeout_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context,
    &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ],
    &now
  );
}
