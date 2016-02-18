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

#include <rtems/system.h>
#include <rtems/seterr.h>
#include <rtems/score/thread.h>
#include <rtems/posix/timerimpl.h>

int timer_getoverrun(
  timer_t   timerid
)
{
  int                  overrun;
  POSIX_Timer_Control *ptimer;
  Objects_Locations    location;
  ISR_lock_Context     lock_context;
  Per_CPU_Control     *cpu;

  ptimer = _POSIX_Timer_Get( timerid, &location, &lock_context );
  switch ( location ) {

    case OBJECTS_LOCAL:
      cpu = _POSIX_Timer_Acquire_critical( ptimer, &lock_context );
      overrun = ptimer->overrun;
      ptimer->overrun = 0;
      _POSIX_Timer_Release( cpu, &lock_context );
      return overrun;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  rtems_set_errno_and_return_minus_one( EINVAL );
}
