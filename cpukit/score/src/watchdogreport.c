/**
 *  @file watchdogreport.c
 *
 *  @brief Report Information on a Single Watchdog Instance
 *  @ingroup ScoreWatchdog
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
#include <rtems/score/watchdogimpl.h>
#include <rtems/bspIo.h>

void _Watchdog_Report(
  const char        *name,
  Watchdog_Control  *watch
)
{
  printk(
    "%s%s%4d %5d %p %p 0x%08x %p\n",
    ((name) ? name : ""),
    ((name) ? " "  : ""),
    watch->delta_interval,
    watch->initial,
    watch,
    watch->routine,
    watch->id,
    watch->user_data
  );
}
