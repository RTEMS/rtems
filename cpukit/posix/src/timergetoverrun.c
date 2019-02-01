/**
 *  @file
 *
 *  @brief Get Overrun Count for a POSIX Per-Process Timer
 *  @ingroup POSIX_PRIV_TIMERS
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

#include <time.h>
#include <errno.h>

#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/timerimpl.h>

int timer_getoverrun(
  timer_t   timerid
)
{
  POSIX_Timer_Control *ptimer;
  ISR_lock_Context     lock_context;

  ptimer = _POSIX_Timer_Get( timerid, &lock_context );
  if ( ptimer != NULL ) {
    Per_CPU_Control *cpu;
    int              overrun;

    cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );
    overrun = ptimer->overrun;
    ptimer->overrun = 0;
    _POSIX_Timer_Release( cpu, &lock_context );
    return overrun;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
