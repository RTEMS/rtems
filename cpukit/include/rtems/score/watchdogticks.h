/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief Constants for the watchdog ticks.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_WATCHDOGTICKS_H
#define _RTEMS_SCORE_WATCHDOGTICKS_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreWatchdog
 *
 * @{
 */

/**
 *  @brief Type is used to specify the length of intervals.
 *
 *  This type is used to specify the length of intervals.
 */
typedef uint32_t   Watchdog_Interval;

/**
 * @brief Special watchdog ticks value to indicate an infinite wait.
 */
#define WATCHDOG_NO_TIMEOUT 0

/**
 * @brief The watchdog ticks counter.
 *
 * With a 1ms watchdog tick, this counter overflows after 50 days since boot.
 */
extern volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/**
 * @brief The watchdog nanoseconds per tick.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Nanoseconds_per_tick;

/**
 * @brief The watchdog ticks per second.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Ticks_per_second;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
