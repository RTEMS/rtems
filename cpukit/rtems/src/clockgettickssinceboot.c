/**
 *  @file
 *
 *  @brief Obtain Ticks Since Boot
 *  @ingroup ClassicClock
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/watchdogimpl.h>

rtems_interval rtems_clock_get_ticks_since_boot(void)
{
  return _Watchdog_Ticks_since_boot;
}
