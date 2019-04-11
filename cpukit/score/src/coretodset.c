/**
 * @file
 *
 * @brief Set Time of Day Given a Timestamp
 *
 * @ingroup RTEMSScoreTOD
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
  const struct timespec *tod,
  ISR_lock_Context      *lock_context
)
{
  struct bintime  tod_as_bintime;
  uint64_t        tod_as_ticks;
  uint32_t        cpu_max;
  uint32_t        cpu_index;

  _Assert( _TOD_Is_owner() );

  timespec2bintime( tod, &tod_as_bintime );
  _Timecounter_Set_clock( &tod_as_bintime, lock_context );

  tod_as_ticks = _Watchdog_Ticks_from_timespec( tod );
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control  *cpu;
    Watchdog_Header  *header;
    ISR_lock_Context  lock_context;
    Watchdog_Control *first;

    cpu = _Per_CPU_Get_by_index( cpu_index );
    header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];

    _ISR_lock_ISR_disable_and_acquire( &cpu->Watchdog.Lock, &lock_context );

    first = _Watchdog_Header_first( header );

    if ( first != NULL ) {
      _Watchdog_Tickle(
        header,
        first,
        tod_as_ticks,
        &cpu->Watchdog.Lock,
        &lock_context
      );
    }

    _ISR_lock_Release_and_ISR_enable( &cpu->Watchdog.Lock, &lock_context );
  }

  _TOD.is_set = true;
}
