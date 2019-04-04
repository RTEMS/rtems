/**
 *  @file
 *
 *  @brief Watchdog Ticks Since Boot
 *  @ingroup RTEMSScoreWatchdog
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/watchdogticks.h>

/*
 * This is defined in a separate module to allow the new network to define it
 * with an additional strong reference.  Strong references cannot alias a
 * common symbol.  Since the GCC option -fno-common is not used throughout
 * RTEMS we cannot define it here.
 */
volatile Watchdog_Interval _Watchdog_Ticks_since_boot;
