/**
 *  @file
 *
 *  @brief Watchdog Handler Initialization
 *  @ingroup ScoreWatchdog
 */

/*
 *  Watchdog Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/isr.h>
#include <rtems/score/watchdogimpl.h>

void _Watchdog_Handler_initialization( void )
{
  _Watchdog_Sync_count = 0;
  _Watchdog_Sync_level = 0;
  _Watchdog_Ticks_since_boot = 0;

  _Chain_Initialize_empty( &_Watchdog_Ticks_chain );
  _Chain_Initialize_empty( &_Watchdog_Seconds_chain );
}
