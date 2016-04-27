/**
 * @file
 *
 * @brief Set Time of Day Given a Timestamp
 *
 * @ingroup ScoreTOD
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/todimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/watchdogimpl.h>

void _TOD_Set(
  const Timestamp_Control *tod_as_timestamp,
  ISR_lock_Context        *lock_context
)
{
  struct timespec tod_as_timespec;
  uint64_t        tod_as_ticks;
  uint32_t        cpu_count;
  uint32_t        cpu_index;

  _Assert( _API_Mutex_Is_owner( _Once_Mutex ) );

  _Timecounter_Set_clock( tod_as_timestamp, lock_context );

  _Timestamp_To_timespec( tod_as_timestamp, &tod_as_timespec );
  tod_as_ticks = _Watchdog_Ticks_from_timespec( &tod_as_timespec );
  cpu_count = _SMP_Get_processor_count();

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    _Watchdog_Per_CPU_tickle_absolute( cpu, tod_as_ticks );
  }

  _TOD.is_set = true;
}
