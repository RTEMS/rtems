/**
 * @file
 *
 * @brief Function Fetches State of POSIX Per-Process Timers
 * @ingroup POSIXAPI
 */

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>
#include <errno.h>

#include <rtems/posix/timerimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/seterr.h>

/*
 *          - When a timer is initialized, the value of the time in
 *            that moment is stored.
 *          - When this function is called, it returns the difference
 *            between the current time and the initialization time.
 */

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context     lock_context;
  uint64_t             now;
  uint32_t             remaining;

  if ( !value )
    rtems_set_errno_and_return_minus_one( EINVAL );

  ptimer = _POSIX_Timer_Get( timerid, &lock_context );
  if ( ptimer != NULL ) {
    Per_CPU_Control *cpu;

    cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );
    now = cpu->Watchdog.ticks;

    if ( now < ptimer->Timer.expire ) {
      remaining = (uint32_t) ( ptimer->Timer.expire - now );
    } else {
      remaining = 0;
    }

    _Timespec_From_ticks( remaining, &value->it_value );
    value->it_interval = ptimer->timer_data.it_interval;

    _POSIX_Timer_Release( cpu, &lock_context );
    return 0;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
