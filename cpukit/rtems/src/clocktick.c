/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/clock.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  rtems_clock_tick
 *
 *  This directive notifies the executve that a tick has occurred.
 *  When the tick occurs the time manager updates and maintains
 *  the calendar time, timeslicing, and any timeout delays.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always succeeds
 *
 *  NOTE: This routine only works for leap-years through 2099.
 */

rtems_status_code rtems_clock_tick( void )
{
  _TOD_Tickle_ticks();

  _Watchdog_Tickle_ticks();

  _Scheduler_Tick();

  if ( _Thread_Is_context_switch_necessary() &&
       _Thread_Is_dispatching_enabled() )
    _Thread_Dispatch();

  return RTEMS_SUCCESSFUL;
}
