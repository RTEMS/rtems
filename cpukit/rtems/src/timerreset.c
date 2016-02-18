/**
 * @file
 *
 * @brief RTEMS Timer Reset
 * @ingroup ClassicTimer Timers
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timerimpl.h>
#include <rtems/score/watchdogimpl.h>

/*
 *  rtems_timer_reset
 *
 *  This directive allows a thread to reset a timer.
 *
 *  Input parameters:
 *    id - timer id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_timer_reset(
  rtems_id id
)
{
  Timer_Control     *the_timer;
  Objects_Locations  location;
  ISR_lock_Context   lock_context;
  Per_CPU_Control   *cpu;
  rtems_status_code  status;

  the_timer = _Timer_Get( id, &location, &lock_context );
  switch ( location ) {

    case OBJECTS_LOCAL:
      cpu = _Timer_Acquire_critical( the_timer, &lock_context );

      if ( _Timer_Is_interval_class( the_timer->the_class ) ) {
        _Timer_Cancel( cpu, the_timer );
        _Watchdog_Insert(
          &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_RELATIVE ],
          &the_timer->Ticker,
          cpu->Watchdog.ticks + the_timer->initial
        );
        status = RTEMS_SUCCESSFUL;
      } else {
        status = RTEMS_NOT_DEFINED;
      }

      _Timer_Release( cpu, &lock_context );
      return status;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:            /* should never return this */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return RTEMS_INVALID_ID;
}
