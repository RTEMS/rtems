/**
 *  @file
 *
 *  @brief Announce a Clock Tick
 *  @ingroup ClassicClock
 */

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

#include <rtems/rtems/clock.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

rtems_status_code rtems_clock_tick( void )
{
#if defined( RTEMS_SMP )
  _Thread_Disable_dispatch();
#endif

  _TOD_Tickle_ticks();

  _Watchdog_Tickle_ticks();

  _Scheduler_Tick();

#if defined( RTEMS_SMP )
  _Thread_Enable_dispatch();
#else
  if ( _Thread_Is_context_switch_necessary() &&
       _Thread_Dispatch_is_enabled() )
    _Thread_Dispatch();
#endif

  return RTEMS_SUCCESSFUL;
}
