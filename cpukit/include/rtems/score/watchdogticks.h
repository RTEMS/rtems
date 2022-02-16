/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreWatchdog related to watchdog ticks which are used by the
 *   implementation and the @ref RTEMSImplApplConfig.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @brief Default value for the watchdog ticks per timeslice.
 */
#define WATCHDOG_TICKS_PER_TIMESLICE_DEFAULT 50

/**
 * @brief The watchdog ticks counter.
 *
 * With a 1ms watchdog tick, this counter overflows after 50 days since boot.
 */
extern volatile Watchdog_Interval _Watchdog_Ticks_since_boot;

/**
 * @brief The watchdog microseconds per tick.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Microseconds_per_tick;

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

/**
 * @brief The watchdog ticks per timeslice.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uint32_t _Watchdog_Ticks_per_timeslice;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
