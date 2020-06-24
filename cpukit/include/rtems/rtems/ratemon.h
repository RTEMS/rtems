/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the Rate-Monotonic Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 2017 Kuan-Hsun Chen
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

/* Generated from spec:/rtems/ratemon/if/header */

#ifndef _RTEMS_RTEMS_RATEMON_H
#define _RTEMS_RTEMS_RATEMON_H

#include <stdint.h>
#include <sys/_timespec.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/score/watchdogticks.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/ratemon/if/group */

/**
 * @defgroup RTEMSAPIClassicRatemon Rate-Monotonic Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Rate-Monotonic Manager provides facilities to implement tasks
 *   which execute in a periodic fashion.  Critically, it also gathers
 *   information about the execution of those periods and can provide important
 *   statistics to the user which can be used to analyze and tune the
 *   application.
 */

/* Generated from spec:/rtems/ratemon/if/period-states */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief This enumeration defines the states in which a period may be.
 */
typedef enum {
  /**
   * @brief This status indicates the period is off the watchdog chain, and has
   *   never been initialized.
   */
  RATE_MONOTONIC_INACTIVE,

  /**
   * @brief This status indicates the period is on the watchdog chain, and
   *   running.  The owner may be executing or blocked waiting on another object.
   */
  RATE_MONOTONIC_ACTIVE,

  /**
   * @brief This status indicates the period is off the watchdog chain, and has
   *   expired. The owner may still execute and has taken too much time to
   *   complete this iteration of the period.
   */
  RATE_MONOTONIC_EXPIRED
} rtems_rate_monotonic_period_states;

/* Generated from spec:/rtems/ratemon/if/period-statistics */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief This structure provides the statistics of a period.
 */
typedef struct {
  /**
   * @brief This member contains the number of periods executed.
   */
  uint32_t count;

  /**
   * @brief This member contains the number of periods missed.
   */
  uint32_t missed_count;

  /**
   * @brief This member contains the least amount of processor time used in a
   *   period.
   */
  struct timespec min_cpu_time;

  /**
   * @brief This member contains the highest amount of processor time used in a
   *   period.
   */
  struct timespec max_cpu_time;

  /**
   * @brief This member contains the total amount of processor time used in a
   *   period.
   */
  struct timespec total_cpu_time;

  /**
   * @brief This member contains the least amount of CLOCK_MONOTONIC time used in
   *   a period.
   */
  struct timespec min_wall_time;

  /**
   * @brief This member contains the highest amount of CLOCK_MONOTONIC time used
   *   in a period.
   */
  struct timespec max_wall_time;

  /**
   * @brief This member contains the total amount of CLOCK_MONOTONIC time used in
   *   a period.
   */
  struct timespec total_wall_time;
} rtems_rate_monotonic_period_statistics;

/* Generated from spec:/rtems/ratemon/if/period-status */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief This structure provides the detailed status of a period.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the owner task of the period.
   */
  rtems_id owner;

  /**
   * @brief This member contains the state of the period.
   */
  rtems_rate_monotonic_period_states state;

  /**
   * @brief This member contains the time elapsed since the last successful
   *   invocation rtems_rate_monotonic_period() using CLOCK_MONOTONIC.
   *
   * If the period is expired or has not been initiated, then this value has no
   * meaning.
   */
  struct timespec since_last_period;

  /**
   * @brief This member contains the processor time consumed by the owner task
   *   since the last successful invocation rtems_rate_monotonic_period().
   *
   * If the period is expired or has not been initiated, then this value has no
   * meaning.
   */
  struct timespec executed_since_last_period;

  /**
   * @brief This member contains the count of jobs which are not released yet.
   */
  uint32_t postponed_jobs_count;
} rtems_rate_monotonic_period_status;

/* Generated from spec:/rtems/ratemon/if/period-status-define */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief This constant is the interval passed to the
 *   rtems_rate_monotonic_period() directive to obtain status information.
 */
#define RTEMS_PERIOD_STATUS WATCHDOG_NO_TIMEOUT

/* Generated from spec:/rtems/ratemon/if/printer */

/* Forward declaration */
struct rtems_printer;

/* Generated from spec:/rtems/ratemon/if/create */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Creates a period.
 *
 * @param name is the object name of the period.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the identifier of the created period will be
 *   stored in this variable.
 *
 * This directive creates a period which resides on the local node.  The period
 * has the user-defined object name specified in ``name`` The assigned object
 * identifier is returned in ``id``.  This identifier is used to access the
 * period with other rate monotonic related directives.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was invalid.
 *
 * @retval ::RTEMS_TOO_MANY There was no inactive object available to create a
 *   period.  The number of periods available to the application is configured
 *   through the #CONFIGURE_MAXIMUM_PERIODS application configuration option.
 *
 * @par Notes
 * @parblock
 * The calling task is registered as the owner of the created period.  Some
 * directives can be only used by this task for the created period.
 *
 * For control and maintenance of the period, RTEMS allocates a PCB from the
 * local PCB free pool and initializes it.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The number of periods available to the application is configured through
 *   the #CONFIGURE_MAXIMUM_PERIODS application configuration option.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may allocate memory from the RTEMS
 *   Workspace.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_create( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/ratemon/if/ident */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Identifies a period by the object name.
 *
 * @param name is the object name to look up.
 *
 * @param[out] id is the pointer to an object identifier variable.  When the
 *   directive call is successful, the object identifier of an object with the
 *   specified name will be stored in this variable.
 *
 * This directive obtains a period identifier associated with the period name
 * specified in ``name``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``id`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_NAME The ``name`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_NAME There was no object with the specified name on
 *   the local node.
 *
 * @par Notes
 * @parblock
 * If the period name is not unique, then the period identifier will match the
 * first period with that name in the search order.  However, this period
 * identifier is not guaranteed to correspond to the desired period.
 *
 * The objects are searched from lowest to the highest index.  Only the local
 * node is searched.
 *
 * The period identifier is used with other rate monotonic related directives
 * to access the period.
 * @endparblock
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
rtems_status_code rtems_rate_monotonic_ident( rtems_name name, rtems_id *id );

/* Generated from spec:/rtems/ratemon/if/cancel */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Cancels the period.
 *
 * @param id is the rate monotonic period identifier.
 *
 * This directive cancels the rate monotonic period specified by ``id``.  This
 * period may be reinitiated by the next invocation of
 * rtems_rate_monotonic_period().
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no rate monotonic period associated
 *   with the identifier specified by ``id``.
 *
 * @retval ::RTEMS_NOT_OWNER_OF_RESOURCE The rate monotonic period was not
 *   created by the calling task.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 *
 * * The directive may be used exclusively by the task which created the
 *   associated object.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_cancel( rtems_id id );

/* Generated from spec:/rtems/ratemon/if/delete */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Deletes the period.
 *
 * @param id is the period identifier.
 *
 * This directive deletes the period specified by ``id``.  If the period is
 * running, it is automatically canceled.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no period associated with the
 *   identifier specified by ``id``.
 *
 * @par Notes
 * The PCB for the deleted period is reclaimed by RTEMS.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The calling task does not have to be the task that created the object.
 *   Any local task that knows the object identifier can delete the object.
 *
 * * Where the object class corresponding to the directive is configured to use
 *   unlimited objects, the directive may free memory to the RTEMS Workspace.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_delete( rtems_id id );

/* Generated from spec:/rtems/ratemon/if/period */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Concludes the current period and start the next period, or gets the
 *   period status.
 *
 * @param id is the rate monotonic period identifier.
 *
 * @param length is the period length in clock ticks or #RTEMS_PERIOD_STATUS to
 *   get the period status.
 *
 * This directive initiates the rate monotonic period specified by ``id``  with
 * a length of period ticks specified by ``length``.  If the period is running,
 * then the calling task will block for the remainder of the period before
 * reinitiating the period with the specified period length.  If the period was
 * not running (either expired or never initiated), the period is immediately
 * initiated and the directive returns immediately.  If the period has expired,
 * the postponed job will be released immediately and the following calls of
 * this directive will release postponed jobs until there is no more deadline
 * miss.
 *
 * If invoked with a period length of #RTEMS_PERIOD_STATUS ticks, the current
 * state of the period will be returned.  The directive status indicates the
 * current state of the period.  This does not alter the state or period length
 * of the period.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no rate monotonic period associated
 *   with the identifier specified by ``id``.
 *
 * @retval ::RTEMS_NOT_OWNER_OF_RESOURCE The rate monotonic period was not
 *   created by the calling task.
 *
 * @retval ::RTEMS_NOT_DEFINED The rate monotonic period has never been
 *   initiated (only possible when the ``length`` parameter was equal to
 *   #RTEMS_PERIOD_STATUS).
 *
 * @retval ::RTEMS_TIMEOUT The rate monotonic period has expired.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be used exclusively by the task which created the
 *   associated object.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_period(
  rtems_id       id,
  rtems_interval length
);

/* Generated from spec:/rtems/ratemon/if/get-status */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Gets the detailed status of the period.
 *
 * @param id is the rate monotonic period identifier.
 *
 * @param[out] status is the pointer to a rtems_rate_monotonic_period_status
 *   variable.  When the directive call is successful, the detailed period
 *   status will be stored in this variable.
 *
 * This directive returns the detailed status of the rate monotonic period
 * specified by ``id``.  The detailed status of the period will be returned in
 * the members of the period status object referenced by ``status``:
 *
 * * The ``owner`` member is set to the identifier of the owner task of the
 *   period.
 *
 * * The ``state`` member is set to the current state of the period.
 *
 * * The ``postponed_jobs_count`` member is set to the count of jobs which are
 *   not released yet.
 *
 * * If the current state of the period is ::RATE_MONOTONIC_INACTIVE, the
 *   ``since_last_period`` and ``executed_since_last_period`` members will be
 *   set to zero.  Otherwise, both members will contain time information since
 *   the last successful invocation of the rtems_rate_monotonic_period()
 *   directive by the owner task.  More specifically, the ``since_last_period``
 *   member will be set to the time elapsed since the last successful
 *   invocation.  The ``executed_since_last_period`` member will be set to the
 *   processor time consumed by the owner task since the last successful
 *   invocation.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no rate monotonic period associated
 *   with the identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``status`` parameter was NULL.
 *
 * @retval ::RTEMS_NOT_DEFINED There was no status available due to a reset of
 *   the processor time usage of the owner task of the period.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                            id,
  rtems_rate_monotonic_period_status *status
);

/* Generated from spec:/rtems/ratemon/if/get-statistics */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Gets the statistics of the period.
 *
 * @param id is the rate monotonic period identifier.
 *
 * @param[out] status is the pointer to a
 *   rtems_rate_monotonic_period_statistics variable.  When the directive call
 *   is successful, the period statistics will be stored in this variable.
 *
 * This directive returns the statistics of the rate monotonic period specified
 * by ``id``.  The statistics of the period will be returned in the members of
 * the period statistics object referenced by ``status``:
 *
 * * The ``count`` member is set to the number of periods executed.
 *
 * * The ``missed_count`` member is set to the number of periods missed.
 *
 * * The ``min_cpu_time`` member is set to the least amount of processor time
 *   used in the period.
 *
 * * The ``max_cpu_time`` member is set to the highest amount of processor time
 *   used in the period.
 *
 * * The ``total_cpu_time`` member is set to the total amount of processor time
 *   used in the period.
 *
 * * The ``min_wall_time`` member is set to the least amount of CLOCK_MONOTONIC
 *   time used in the period.
 *
 * * The ``max_wall_time`` member is set to the highest amount of
 *   CLOCK_MONOTONIC time used in the period.
 *
 * * The ``total_wall_time`` member is set to the total amount of
 *   CLOCK_MONOTONIC time used in the period.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no rate monotonic period associated
 *   with the identifier specified by ``id``.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``status`` parameter was NULL.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_get_statistics(
  rtems_id                                id,
  rtems_rate_monotonic_period_statistics *status
);

/* Generated from spec:/rtems/ratemon/if/reset-statistics */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Resets the statistics of the period.
 *
 * @param id is the rate monotonic period identifier.
 *
 * This directive resets the statistics of the rate monotonic period specified
 * by ``id``.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no rate monotonic period associated
 *   with the identifier specified by ``id``.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_rate_monotonic_reset_statistics( rtems_id id );

/* Generated from spec:/rtems/ratemon/if/reset-all-statistics */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Resets the statistics of all periods.
 *
 * This directive resets the statistics information associated with all rate
 * monotonic period instances.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_rate_monotonic_reset_all_statistics( void );

/* Generated from spec:/rtems/ratemon/if/report-statistics */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Reports the period statistics using the printk() printer.
 *
 * This directive prints a report on all active periods which have executed at
 * least one period using the printk() printer.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_rate_monotonic_report_statistics( void );

/* Generated from spec:/rtems/ratemon/if/report-statistics-with-plugin */

/**
 * @ingroup RTEMSAPIClassicRatemon
 *
 * @brief Reports the period statistics using the printer plugin.
 *
 * @param printer is the printer plugin to output the report.
 *
 * This directive prints a report on all active periods which have executed at
 * least one period using the printer plugin specified by ``printer``.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_rate_monotonic_report_statistics_with_plugin(
  const struct rtems_printer *printer
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_RATEMON_H */
