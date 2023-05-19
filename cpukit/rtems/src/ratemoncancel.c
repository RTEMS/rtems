/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRateMonotonic
 *
 * @brief This source file contains the implementation of
 *   rtems_rate_monotonic_cancel().
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/rtems/ratemonimpl.h>

void _Rate_monotonic_Cancel(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
)
{
  Per_CPU_Control      *cpu_self;
  Thread_queue_Context  queue_context;

  _Rate_monotonic_Acquire_critical( the_period, lock_context );

  _Watchdog_Per_CPU_remove_ticks( &the_period->Timer );
  the_period->postponed_jobs = 0;
  the_period->state = RATE_MONOTONIC_INACTIVE;
  _Scheduler_Cancel_job(
    the_period->owner,
    &the_period->Priority,
    &queue_context
  );

  cpu_self = _Thread_Dispatch_disable_critical( lock_context );
  _Rate_monotonic_Release( the_period, lock_context );
  _Thread_Priority_update( &queue_context );
  _Thread_Dispatch_enable( cpu_self );
}

rtems_status_code rtems_rate_monotonic_cancel(
  rtems_id id
)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;
  Thread_Control         *executing;

  the_period = _Rate_monotonic_Get( id, &lock_context );
  if ( the_period == NULL ) {
    return RTEMS_INVALID_ID;
  }

  executing = _Thread_Executing;
  if ( executing != the_period->owner ) {
    _ISR_lock_ISR_enable( &lock_context );
    return RTEMS_NOT_OWNER_OF_RESOURCE;
  }

  _Rate_monotonic_Cancel( the_period, executing, &lock_context );
  return RTEMS_SUCCESSFUL;
}
