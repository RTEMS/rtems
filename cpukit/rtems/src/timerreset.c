/**
 * @file
 *
 * @ingroup ClassicTimer Timers
 *
 * @brief RTEMS Timer Reset
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/timerimpl.h>

rtems_status_code rtems_timer_reset(
  rtems_id id
)
{
  Timer_Control    *the_timer;
  ISR_lock_Context  lock_context;

  the_timer = _Timer_Get( id, &lock_context );
  if ( the_timer != NULL ) {
    Per_CPU_Control   *cpu;
    rtems_status_code  status;

    cpu = _Timer_Acquire_critical( the_timer, &lock_context );

    if ( _Timer_Is_interval_class( the_timer->the_class ) ) {
      _Timer_Cancel( cpu, the_timer );
      _Watchdog_Insert(
        &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ],
        &the_timer->Ticker,
        cpu->Watchdog.ticks + the_timer->initial
      );
      status = RTEMS_SUCCESSFUL;
    } else {
      status = RTEMS_NOT_DEFINED;
    }

    _Timer_Release( cpu, &lock_context );
    return status;
  }

  return RTEMS_INVALID_ID;
}
