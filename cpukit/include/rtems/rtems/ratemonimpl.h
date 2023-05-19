/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicRateMonotonic
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicRateMonotonic.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
 *  COPYRIGHT (c) 2016 Kuan-Hsun Chen.
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

#ifndef _RTEMS_RTEMS_RATEMONIMPL_H
#define _RTEMS_RTEMS_RATEMONIMPL_H

#include <rtems/rtems/ratemondata.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicRateMonotonic Rate Monotonic Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Rate Monotonic Manager implementation.
 *
 * @{
 */

#define RATE_MONOTONIC_INTEND_TO_BLOCK \
  ( THREAD_WAIT_CLASS_PERIOD | THREAD_WAIT_STATE_INTEND_TO_BLOCK )

#define RATE_MONOTONIC_BLOCKED \
  ( THREAD_WAIT_CLASS_PERIOD | THREAD_WAIT_STATE_BLOCKED )

/**
 *  @brief Allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 */
static inline Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

static inline void _Rate_monotonic_Acquire_critical(
  Rate_monotonic_Control *the_period,
  ISR_lock_Context       *lock_context
)
{
  _ISR_lock_Acquire( &the_period->Lock, lock_context );
}

static inline void _Rate_monotonic_Release(
  Rate_monotonic_Control *the_period,
  ISR_lock_Context       *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_period->Lock, lock_context );
}

static inline Rate_monotonic_Control *_Rate_monotonic_Get(
  Objects_Id        id,
  ISR_lock_Context *lock_context
)
{
  return (Rate_monotonic_Control *)
    _Objects_Get( id, lock_context, &_Rate_monotonic_Information );
}

void _Rate_monotonic_Timeout( Watchdog_Control *watchdog );

/**
 * @brief Gets the rate monotonic CPU usage status.
 *
 * This routine is invoked to compute the elapsed wall time and cpu
 * time for a period.
 *
 * @param[in] the_period points to the period being operated upon.
 * @param[out] wall_since_last_period is set to the wall time elapsed
 *             since the period was initiated.
 * @param[out] cpu_since_last_period is set to the cpu time used by the
 *             owning thread since the period was initiated.
 */
void _Rate_monotonic_Get_status(
  const Rate_monotonic_Control *the_period,
  Timestamp_Control            *wall_since_last_period,
  Timestamp_Control            *cpu_since_last_period
);

void _Rate_monotonic_Restart(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
);

void _Rate_monotonic_Cancel(
  Rate_monotonic_Control *the_period,
  Thread_Control         *owner,
  ISR_lock_Context       *lock_context
);

static inline void _Rate_monotonic_Reset_min_time(
  Timestamp_Control *min_time
)
{
  _Timestamp_Set( min_time, 0x7fffffff, 0x7fffffff );
}

static inline void _Rate_monotonic_Reset_statistics(
  Rate_monotonic_Control *the_period
)
{
  Rate_monotonic_Statistics *statistics;

  statistics = &the_period->Statistics;
  memset( statistics, 0, sizeof( *statistics ) );
  _Rate_monotonic_Reset_min_time( &statistics->min_wall_time );
  _Rate_monotonic_Reset_min_time( &statistics->min_cpu_time );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
