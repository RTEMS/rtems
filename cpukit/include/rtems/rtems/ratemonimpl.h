/**
 * @file
 *
 * @ingroup ClassicRateMonImpl
 *
 * @brief Classic Rate Monotonic Scheduler Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH.
 *  COPYRIGHT (c) 2016 Kuan-Hsun Chen.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
 * @defgroup ClassicRateMonImpl Classic Rate Monotonic Scheduler Implementation
 *
 * @ingroup RTEMSInternalClassic
 *
 * @{
 */

#define RATE_MONOTONIC_INTEND_TO_BLOCK \
  ( THREAD_WAIT_CLASS_PERIOD | THREAD_WAIT_STATE_INTEND_TO_BLOCK )

#define RATE_MONOTONIC_BLOCKED \
  ( THREAD_WAIT_CLASS_PERIOD | THREAD_WAIT_STATE_BLOCKED )

#define RATE_MONOTONIC_READY_AGAIN \
  ( THREAD_WAIT_CLASS_PERIOD | THREAD_WAIT_STATE_READY_AGAIN )

/**
 *  @brief Allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

RTEMS_INLINE_ROUTINE void _Rate_monotonic_Acquire_critical(
  Rate_monotonic_Control *the_period,
  ISR_lock_Context       *lock_context
)
{
  _ISR_lock_Acquire( &the_period->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _Rate_monotonic_Release(
  Rate_monotonic_Control *the_period,
  ISR_lock_Context       *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_period->Lock, lock_context );
}

RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Get(
  Objects_Id        id,
  ISR_lock_Context *lock_context
)
{
  return (Rate_monotonic_Control *)
    _Objects_Get( id, lock_context, &_Rate_monotonic_Information );
}

void _Rate_monotonic_Timeout( Watchdog_Control *watchdog );

/**
 * @brief _Rate_monotonic_Get_status(
 *
 * This routine is invoked to compute the elapsed wall time and cpu
 * time for a period.
 *
 * @param[in] the_period points to the period being operated upon.
 * @param[out] wall_since_last_period is set to the wall time elapsed
 *             since the period was initiated.
 * @param[out] cpu_since_last_period is set to the cpu time used by the
 *             owning thread since the period was initiated.
 *
 * @retval This routine returns true if the status can be determined
 *         and false otherwise.
 */
bool _Rate_monotonic_Get_status(
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

RTEMS_INLINE_ROUTINE void _Rate_monotonic_Reset_min_time(
  Timestamp_Control *min_time
)
{
  _Timestamp_Set( min_time, 0x7fffffff, 0x7fffffff );
}

RTEMS_INLINE_ROUTINE void _Rate_monotonic_Reset_statistics(
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
