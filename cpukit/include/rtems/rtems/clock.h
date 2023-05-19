/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicClock
 *
 * @brief This header file defines the Clock Manager API.
 */

/*
 * Copyright (C) 2014, 2021 embedded brains GmbH & Co. KG
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

/* Generated from spec:/rtems/clock/if/bintime */

/* Forward declaration */
struct bintime;

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
 * RTEMS can represent time points of the CLOCK_REALTIME clock in nanoseconds
 * ranging from 1988-01-01T00:00:00.000000000Z to
 * 2514-05-31T01:53:03.999999999Z.  The future uptime of the system shall be in
 * this range, otherwise the system behaviour is undefined.  Due to
 * implementation constraints, the time of day set by the directive shall be
 * before 2100-01-01:00:00.000000000Z.  The latest valid time of day accepted
 * by the POSIX clock_settime() is 2400-01-01T00:00:00.999999999Z.
 *
 * The specified time is based on the configured clock tick rate, see the @ref
 * CONFIGURE_MICROSECONDS_PER_TICK application configuration option.
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
 * * The directive may change the priority of a task.  This may cause the
 *   calling task to be preempted.
 *
 * * The directive may unblock a task.  This may cause the calling task to be
 *   preempted.
 *
 * * The time of day set by the directive shall be
 *   1988-01-01T00:00:00.000000000Z or later.
 *
 * * The time of day set by the directive shall be before
 *   2100-01-01T00:00:00.000000000Z.
 * @endparblock
 */
rtems_status_code rtems_clock_set( const rtems_time_of_day *time_of_day );

/* Generated from spec:/rtems/clock/if/get-tod */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time of day associated with the current CLOCK_REALTIME.
 *
 * @param time_of_day is the pointer to an rtems_time_of_day object.  When the
 *   directive call is successful, the time of day associated with the
 *   CLOCK_REALTIME at some point during the directive call will be stored in
 *   this object.
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
 * @param[out] time_of_day is the pointer to a struct timeval object.  When the
 *   directive call is successful, the seconds and microseconds elapsed since
 *   the Unix epoch and the CLOCK_REALTIME at some point during the directive
 *   call will be stored in this object.
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

/* Generated from spec:/rtems/clock/if/get-realtime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in seconds and nanoseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timespec object.  The
 *   time elapsed since the Unix epoch measured using the CLOCK_REALTIME at
 *   some time point during the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_realtime_coarse() directive may be used to get the time in a
 * lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_realtime_bintime() and
 * rtems_clock_get_realtime_timeval() to get the time in alternative formats.
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
void rtems_clock_get_realtime( struct timespec *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-realtime-bintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in binary time format.
 *
 * @param[out] time_snapshot is the pointer to a bintime object.  The time
 *   elapsed since the Unix epoch measured using the CLOCK_REALTIME at some
 *   time point during the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_realtime_coarse_bintime() directive may be used to get the
 * time in a lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_realtime() and rtems_clock_get_realtime_timeval() to get
 * the time in alternative formats.
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
void rtems_clock_get_realtime_bintime( struct bintime *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-realtime-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in seconds and microseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timeval object.  The
 *   time elapsed since the Unix epoch measured using the CLOCK_REALTIME at
 *   some time point during the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_realtime_coarse_timeval() directive may be used to get the
 * time in a lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_realtime() and rtems_clock_get_realtime_bintime() to get
 * the time in alternative formats.
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
void rtems_clock_get_realtime_timeval( struct timeval *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-realtime-coarse */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in coarse resolution in seconds and nanoseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timespec object.  The
 *   time elapsed since the Unix epoch measured using the CLOCK_REALTIME at
 *   some time point close to the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_realtime() directive may be used to get the time in a higher
 * resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_realtime_coarse_bintime() and
 * rtems_clock_get_realtime_coarse_timeval() to get the time in alternative
 * formats.
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
void rtems_clock_get_realtime_coarse( struct timespec *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-realtime-coarse-bintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in coarse resolution in binary time format.
 *
 * @param[out] time_snapshot is the pointer to a bintime object.  The time
 *   elapsed since the Unix epoch measured using the CLOCK_REALTIME at some
 *   time point close to the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_realtime_bintime() directive may be used to get the time in
 * a higher resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_realtime_coarse() and
 * rtems_clock_get_realtime_coarse_timeval() to get the time in alternative
 * formats.
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
void rtems_clock_get_realtime_coarse_bintime( struct bintime *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-realtime-coarse-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch measured using
 *   CLOCK_REALTIME in coarse resolution in seconds and microseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timeval object.  The
 *   time elapsed since the Unix epoch measured using the CLOCK_REALTIME at
 *   some time point close to the directive call will be stored in this object.
 *   Calling the directive with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_realtime_timeval() directive may be used to get the time in
 * a higher resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_realtime_coarse() and
 * rtems_clock_get_realtime_coarse_timeval() to get the time in alternative
 * formats.
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
void rtems_clock_get_realtime_coarse_timeval( struct timeval *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in seconds and nanoseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timespec object.  The
 *   time elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point during the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_monotonic_coarse() directive may be used to get the time
 * with in a lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_monotonic_bintime(),
 * rtems_clock_get_monotonic_sbintime(), and
 * rtems_clock_get_monotonic_timeval() to get the time in alternative formats.
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
void rtems_clock_get_monotonic( struct timespec *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic-bintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in binary time format.
 *
 * @param[out] time_snapshot is the pointer to a bintime object.  The time
 *   elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point during the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_monotonic_coarse_bintime() directive may be used to get the
 * time in a lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_monotonic(), rtems_clock_get_monotonic_sbintime(), and
 * rtems_clock_get_monotonic_timeval() to get the time in alternative formats.
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
void rtems_clock_get_monotonic_bintime( struct bintime *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic-sbintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in signed binary time format.
 *
 * @return Returns the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC at some time point during the directive
 *   call.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.
 *
 * See rtems_clock_get_monotonic(), rtems_clock_get_monotonic_bintime(), and
 * rtems_clock_get_monotonic_timeval() to get the time in alternative formats.
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
int64_t rtems_clock_get_monotonic_sbintime( void );

/* Generated from spec:/rtems/clock/if/get-monotonic-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in seconds and microseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timeval object.  The
 *   time elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point during the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive accesses a device provided by the Clock Driver to get the time
 * in the highest resolution available to the system.  Alternatively, the
 * rtems_clock_get_monotonic_coarse_timeval() directive may be used to get the
 * time in a lower resolution and with less runtime overhead.
 *
 * See rtems_clock_get_monotonic(), rtems_clock_get_monotonic_bintime(), and
 * rtems_clock_get_monotonic_sbintime() to get the time in alternative formats.
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
void rtems_clock_get_monotonic_timeval( struct timeval *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic-coarse */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in coarse resolution in seconds and
 *   nanoseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timespec object.  The
 *   time elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point close to the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_monotonic() directive may be used to get the time in a
 * higher resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_monotonic_coarse_bintime() and
 * rtems_clock_get_monotonic_coarse_timeval() to get the time in alternative
 * formats.
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
void rtems_clock_get_monotonic_coarse( struct timespec *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic-coarse-bintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in coarse resolution in binary time
 *   format.
 *
 * @param[out] time_snapshot is the pointer to a bintime object.  The time
 *   elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point close to the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_monotonic_bintime() directive may be used to get the time in
 * a higher resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_monotonic_coarse() and
 * rtems_clock_get_monotonic_coarse_timeval() to get the time in alternative
 * formats.
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
void rtems_clock_get_monotonic_coarse_bintime( struct bintime *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-monotonic-coarse-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since some fixed time point in the past
 *   measured using the CLOCK_MONOTONIC in coarse resolution in seconds and
 *   microseconds format.
 *
 * @param[out] time_snapshot is the pointer to a struct timeval object.  The
 *   time elapsed since some fixed time point in the past measured using the
 *   CLOCK_MONOTONIC at some time point close to the directive call will be
 *   stored in this object.  Calling the directive with a pointer equal to NULL
 *   is undefined behaviour.
 *
 * @par Notes
 * @parblock
 * The directive does not access a device to get the time.  It uses a recent
 * snapshot provided by the Clock Driver.  Alternatively, the
 * rtems_clock_get_monotonic_timeval() directive may be used to get the time in
 * a higher resolution and with a higher runtime overhead.
 *
 * See rtems_clock_get_monotonic_coarse() and
 * rtems_clock_get_monotonic_coarse_bintime() to get the time in alternative
 * formats.
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
void rtems_clock_get_monotonic_coarse_timeval( struct timeval *time_snapshot );

/* Generated from spec:/rtems/clock/if/get-boot-time */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch at some time point during
 *   system initialization in seconds and nanoseconds format.
 *
 * @param[out] boot_time is the pointer to a struct timespec object.  The time
 *   elapsed since the Unix epoch at some time point during system
 *   initialization call will be stored in this object.  Calling the directive
 *   with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * See rtems_clock_get_boot_time_bintime() and
 * rtems_clock_get_boot_time_timeval() to get the boot time in alternative
 * formats.  Setting the CLOCK_REALTIME will also set the boot time.
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
void rtems_clock_get_boot_time( struct timespec *boot_time );

/* Generated from spec:/rtems/clock/if/get-boot-time-bintime */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch at some time point during
 *   system initialization in binary time format.
 *
 * @param[out] boot_time is the pointer to a bintime object.  The time elapsed
 *   since the Unix epoch at some time point during system initialization call
 *   will be stored in this object.  Calling the directive with a pointer equal
 *   to NULL is undefined behaviour.
 *
 * @par Notes
 * See rtems_clock_get_boot_time() and rtems_clock_get_boot_time_timeval() to
 * get the boot time in alternative formats.  Setting the CLOCK_REALTIME will
 * also set the boot time.
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
void rtems_clock_get_boot_time_bintime( struct bintime *boot_time );

/* Generated from spec:/rtems/clock/if/get-boot-time-timeval */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the time elapsed since the Unix epoch at some time point during
 *   system initialization in seconds and microseconds format.
 *
 * @param[out] boot_time is the pointer to a struct timeval object.  The time
 *   elapsed since the Unix epoch at some time point during system
 *   initialization call will be stored in this object.  Calling the directive
 *   with a pointer equal to NULL is undefined behaviour.
 *
 * @par Notes
 * See rtems_clock_get_boot_time() and rtems_clock_get_boot_time_bintime() to
 * get the boot time in alternative formats.  Setting the CLOCK_REALTIME will
 * also set the boot time.
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
void rtems_clock_get_boot_time_timeval( struct timeval *boot_time );

/* Generated from spec:/rtems/clock/if/get-seconds-since-epoch */

/**
 * @ingroup RTEMSAPIClassicClock
 *
 * @brief Gets the seconds elapsed since the RTEMS epoch and the current
 *   CLOCK_REALTIME.
 *
 * @param[out] seconds_since_rtems_epoch is the pointer to an ::rtems_interval
 *   object.  When the directive call is successful, the seconds elapsed since
 *   the RTEMS epoch and the CLOCK_REALTIME at some point during the directive
 *   call will be stored in this object.
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
 * The number of clock ticks per second is defined indirectly by the @ref
 * CONFIGURE_MICROSECONDS_PER_TICK configuration option.
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
 * @param[out] uptime is the pointer to a struct timespec object.  When the
 *   directive call is successful, the seconds and nanoseconds elapsed since
 *   some time point during the system initialization and some point during the
 *   directive call using CLOCK_MONOTONIC will be stored in this object.
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
 * @param[out] uptime is the pointer to a struct timeval object.  The seconds
 *   and microseconds elapsed since some time point during the system
 *   initialization and some point during the directive call using
 *   CLOCK_MONOTONIC will be stored in this object.  The pointer shall be
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
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 * @endparblock
 */
rtems_status_code rtems_clock_tick( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_CLOCK_H */
