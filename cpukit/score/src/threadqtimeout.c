/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Copyright (C) 2016, 2021 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  struct timespec        base;

  if ( queue_context->timeout_absolute ) {
    abstime = queue_context->Timeout.arg;
  } else {
    base = *now;
    abstime = _Watchdog_Future_timespec( &base, queue_context->Timeout.arg );
  }

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

  _Timecounter_Nanouptime( &now );
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

  _Timecounter_Nanotime( &now );
  _Thread_queue_Add_timeout_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context,
    &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ],
    &now
  );
}
