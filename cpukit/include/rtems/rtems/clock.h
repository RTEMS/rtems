/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Clock Manager API.
 */

/*
 * Copyright (C) 2014, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/clock/if/header */

#ifndef _RTEMS_RTEMS_CLOCK_H
#define _RTEMS_RTEMS_CLOCK_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <rtems/config.h>
#include <sys/_timespec.h>
#include <sys/_timeval.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/watchdogticks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/clock/if/group */

/**
 * @defgroup RTEMSAPIClassicClock Clock Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Clock Manager provides support for time of day and other time
 *   related capabilities.
 */

/* Generated from spec:/rtems/clock/if/set */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Sets the CLOCK_REALTIME to the time of day.
 *
 * @param time_of_day is the time of day to set the clock.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``time_of_day`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_CLOCK The time of day specified by ``time_of_day``
 *   was invalid.
 *
 * @par Notes
 * @parblock
 * The date, time, and ticks specified by ``time_of_day`` are all
 * range-checked, and an error is returned if any one is out of its valid
 * range.
 *
 * RTEMS can represent time points of this clock in nanoseconds ranging from
 * 1988-01-01T00:00:00.000000000Z to 2514-05-31T01:53:03.999999999Z.  The
 * future uptime of the system shall be in this range, otherwise the system
 * behaviour is undefined.
 *
 * The specified time is based on the configured clock tick rate, see the
 * #CONFIGURE_MICROSECONDS_PER_TICK application configuration option.
 *
 * Setting the time forward will fire all CLOCK_REALTIME timers which are
 * scheduled at a time point before or at the time set by the directive.  This
 * may unblock tasks, which may preempt the calling task. User-provided timer
 * routines will execute in the context of the caller.
 *
 * It is allowed to call this directive from within interrupt context, however,
 * this is not recommended since an arbitrary number of timers may fire.
 *
 * The directive shall be called at least once to enable the service of
 * CLOCK_REALTIME related directives.  If the clock is not set at least once,
 * they may return an error status.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive may change the priority of another task which may preempt
 *   the calling task.
 *
 * * The directive may unblock another task which may preempt the calling task.
 * @endparblock
 */
rtems_status_code rtems_clock_set( const rtems_time_of_day *time_of_day );

/* Generated from spec:/rtems/clock/if/get-tod */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time of day associated with the current CLOCK_REALTIME.
 *
 * @param time_of_day is the pointer to a RTEMS time of day variable.  When the
 *   directive call is successful, the time of day associated with the
 *   CLOCK_REALTIME at some point during the directive call will be stored in
 *   this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``time_of_day`` parameter was NULL.
 *
 * @retval ::RTEMS_NOT_DEFINED The CLOCK_REALTIME was not set.  It can be set
 *   with rtems_clock_set().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
rtems_status_code rtems_clock_get_tod( rtems_time_of_day *time_of_day );

/* Generated from spec:/rtems/clock/if/get-tod-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds and microseconds elapsed since the Unix epoch and
 *   the current CLOCK_REALTIME.
 *
 * @param[out] time_of_day is the pointer to a timeval structure variable.
 *   When the directive call is successful, the seconds and microseconds
 *   elapsed since the Unix epoch and the CLOCK_REALTIME at some point during
 *   the directive call will be stored in this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``time_of_day`` parameter was NULL.
 *
 * @retval ::RTEMS_NOT_DEFINED The CLOCK_REALTIME was not set.  It can be set
 *   with rtems_clock_set().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
rtems_status_code rtems_clock_get_tod_timeval( struct timeval *time_of_day );

/* Generated from spec:/rtems/clock/if/get-seconds-since-epoch */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds elapsed since the RTEMS epoch and the current
 *   CLOCK_REALTIME.
 *
 * @param[out] seconds_since_rtems_epoch is the pointer to an interval
 *   variable.  When the directive call is successful, the seconds elapsed
 *   since the RTEMS epoch and the CLOCK_REALTIME at some point during the
 *   directive call will be stored in this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``seconds_since_rtems_epoch`` parameter
 *   was NULL.
 *
 * @retval ::RTEMS_NOT_DEFINED The CLOCK_REALTIME was not set.  It can be set
 *   with rtems_clock_set().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
rtems_status_code rtems_clock_get_seconds_since_epoch(
  rtems_interval *seconds_since_rtems_epoch
);

/* Generated from spec:/rtems/clock/if/get-ticks-per-second */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the number of clock ticks per second configured for the
 *   application.
 *
 * @return Returns the number of clock ticks per second configured for this
 *   application.
 *
 * @par Notes
 * The number of clock ticks per second is defined indirectly by the
 * #CONFIGURE_MICROSECONDS_PER_TICK configuration option.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_interval rtems_clock_get_ticks_per_second( void );

/* Generated from spec:/rtems/clock/if/get-ticks-per-second-macro */
#define rtems_clock_get_ticks_per_second() _Watchdog_Ticks_per_second

/* Generated from spec:/rtems/clock/if/get-ticks-since-boot */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the number of clock ticks since some time point during the
 *   system initialization or the last overflow of the clock tick counter.
 *
 * @return Returns the number of clock ticks since some time point during the
 *   system initialization or the last overflow of the clock tick counter.
 *
 * @par Notes
 * With a 1ms clock tick, this counter overflows after 50 days since boot.
 * This is the historical measure of uptime in an RTEMS system.  The newer
 * service rtems_clock_get_uptime() is another and potentially more accurate
 * way of obtaining similar information.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_interval rtems_clock_get_ticks_since_boot( void );

/* Generated from spec:/rtems/clock/if/get-ticks-since-boot-macro */
#define rtems_clock_get_ticks_since_boot() _Watchdog_Ticks_since_boot

/* Generated from spec:/rtems/clock/if/get-uptime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds and nanoseconds elapsed since some time point during
 *   the system initialization using CLOCK_MONOTONIC.
 *
 * @param[out] uptime is the pointer to a timeval structure variable.  When the
 *   directive call is successful, the seconds and nanoseconds elapsed since
 *   some time point during the system initialization and some point during the
 *   directive call using CLOCK_MONOTONIC will be stored in this variable.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``uptime`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
rtems_status_code rtems_clock_get_uptime( struct timespec *uptime );

/* Generated from spec:/rtems/clock/if/get-uptime-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds and microseconds elapsed since some time point
 *   during the system initialization using CLOCK_MONOTONIC.
 *
 * @param[out] uptime is the pointer to a timeval structure variable.  The
 *   seconds and microseconds elapsed since some time point during the system
 *   initialization and some point during the directive call using
 *   CLOCK_MONOTONIC will be stored in this variable.  The pointer shall be
 *   valid, otherwise the behaviour is undefined.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
void rtems_clock_get_uptime_timeval( struct timeval *uptime );

/* Generated from spec:/rtems/clock/if/get-uptime-seconds */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds elapsed since some time point during the system
 *   initialization using CLOCK_MONOTONIC.
 *
 * @return Returns the seconds elapsed since some time point during the system
 *   initialization and some point during the directive call using
 *   CLOCK_MONOTONIC.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
time_t rtems_clock_get_uptime_seconds( void );

/* Generated from spec:/rtems/clock/if/get-uptime-nanoseconds */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the nanoseconds elapsed since some time point during the system
 *   initialization using CLOCK_MONOTONIC.
 *
 * @return Returns the nanoseconds elapsed since some time point during the
 *   system initialization and some point during the directive call using
 *   CLOCK_MONOTONIC.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
uint64_t rtems_clock_get_uptime_nanoseconds( void );

/* Generated from spec:/rtems/clock/if/tick-later */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets a clock tick value which is at least delta clock ticks in the
 *   future.
 *
 * @param delta is the delta value in clock ticks.
 *
 * @return Returns a clock tick counter value which is at least ``delta`` clock
 *   ticks in the future.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
static inline rtems_interval rtems_clock_tick_later( rtems_interval delta )
{
  return _Watchdog_Ticks_since_boot + delta;
}

/* Generated from spec:/rtems/clock/if/tick-later-usec */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets a clock tick value which is at least delta microseconds in the
 *   future.
 *
 * @param delta_in_usec is the delta value in microseconds.
 *
 * @return Returns a clock tick counter value which is at least
 *   ``delta_in_usec`` microseconds in the future.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
static inline rtems_interval rtems_clock_tick_later_usec(
  rtems_interval delta_in_usec
)
{
  rtems_interval us_per_tick;

  us_per_tick = rtems_configuration_get_microseconds_per_tick();

  /*
   * Add one additional tick, since we do not know the time to the clock
   * next tick.
   */
  return _Watchdog_Ticks_since_boot + 1
    + ( delta_in_usec + us_per_tick - 1 ) / us_per_tick;
}

/* Generated from spec:/rtems/clock/if/tick-before */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Indicates if the current clock tick counter is before the ticks.
 *
 * @param ticks is the ticks value to check.
 *
 * @return Returns true, if current clock tick counter indicates a time before
 *   the time in ticks, otherwise returns false.
 *
 * @par Notes
 * @parblock
 * This directive can be used to write busy loops with a timeout.
 *
 * @code
 * status busy( void )
 * {
 *   rtems_interval timeout;
 *
 *   timeout = rtems_clock_tick_later_usec( 10000 );
 *
 *   do {
 *     if ( ok() ) {
 *       return success;
 *     }
 *   } while ( rtems_clock_tick_before( timeout ) );
 *
 *   return timeout;
 * }
 * @endcode
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive requires a Clock Driver.
 * @endparblock
 */
static inline bool rtems_clock_tick_before( rtems_interval ticks )
{
  return (int32_t) ( ticks - _Watchdog_Ticks_since_boot ) > 0;
}

/* Generated from spec:/rtems/clock/if/tick */

/**
 * @brief Announces a clock tick.
 *
 * @par Notes
 * The directive is a legacy interface.  It should not be called by
 * applications directly.  A Clock Driver may call this directive.
 */
rtems_status_code rtems_clock_tick( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_CLOCK_H */
