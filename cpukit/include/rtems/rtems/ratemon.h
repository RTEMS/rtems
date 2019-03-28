/**
 * @file
 *
 * @ingroup ClassicRateMon
 *
 * This include file contains all the constants, structures, and
 * prototypes associated with the Rate Monotonic Manager. This manager
 * provides facilities to implement threads which execute in a periodic
 * fashion.
 *
 * Directives provided are:
 *
 * - create a rate monotonic timer
 * - cancel a period
 * - delete a rate monotonic timer
 * - conclude current and start the next period
 * - obtain status information on a period
 * - obtain the number of postponed jobs
 */

/* COPYRIGHT (c) 1989-2009, 2016.
 * On-Line Applications Research Corporation (OAR).
 * COPYRIGHT (c) 2016-2017 Kuan-Hsun Chen.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RATEMON_H
#define _RTEMS_RTEMS_RATEMON_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/status.h>

struct rtems_printer;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicRateMon Rate Monotonic Scheduler
 *
 *  @ingroup RTEMSAPIClassic
 *
 *  This encapsulates functionality related to the Classic API Rate
 *  Monotonic Manager.
 *
 *  Statistics are kept for each period and can be obtained or printed via
 *  API calls.  The statistics kept include minimum, maximum and average times
 *  for both cpu usage and wall time.  The statistics indicate the execution
 *  and wall time used by the owning thread between successive calls to
 *  rtems_rate_monotonic_period.
 */
/**@{*/

typedef struct timespec rtems_rate_monotonic_period_time_t RTEMS_DEPRECATED;

/**
 *  The following enumerated type defines the states in which a
 *  period may be.
 */
typedef enum {
  /**
   * This value indicates the period is off the watchdog chain,
   * and has never been initialized.
   */
  RATE_MONOTONIC_INACTIVE,

  /**
   * This value indicates the period is on the watchdog chain, and
   * running.  The owner should be executed or blocked waiting on
   * another object.
   */
  RATE_MONOTONIC_ACTIVE,

  /**
   * This value indicates the period is off the watchdog chain, and
   * has expired.  The owner is still executing and has taken too much
   * all time to complete this iteration of the period.
   */
  RATE_MONOTONIC_EXPIRED
}   rtems_rate_monotonic_period_states;

/**
 *  The following constant is the interval passed to the rate_monontonic_period
 *  directive to obtain status information.
 */
#define RTEMS_PERIOD_STATUS       WATCHDOG_NO_TIMEOUT

/**
 *  The following defines the PUBLIC data structure that has the
 *  statistics kept on each period instance.
 *
 *  @note The public structure uses struct timespec while the
 *        internal one uses Timestamp_Control.
 */
typedef struct {
  /** This field contains the number of periods executed. */
  uint32_t     count;
  /** This field contains the number of periods missed. */
  uint32_t     missed_count;

  /** This field contains the least amount of CPU time used in a period. */
  struct timespec min_cpu_time;
  /** This field contains the highest amount of CPU time used in a period. */
  struct timespec max_cpu_time;
  /** This field contains the total amount of wall time used in a period. */
  struct timespec total_cpu_time;

  /** This field contains the least amount of wall time used in a period. */
  struct timespec min_wall_time;
  /** This field contains the highest amount of wall time used in a period. */
  struct timespec max_wall_time;
  /** This field contains the total amount of CPU time used in a period. */
  struct timespec total_wall_time;
}  rtems_rate_monotonic_period_statistics;

/**
 *  The following defines the period status structure.
 */
typedef struct {
  /** This is the Id of the thread using this period. */
  rtems_id                             owner;

  /** This is the current state of this period. */
  rtems_rate_monotonic_period_states   state;

  /**
   *  This is the length of wall time that has passed since this period
   *  was last initiated.  If the period is expired or has not been initiated,
   *  then this field has no meaning.
   */
  struct timespec                      since_last_period;

  /**
   *  This is the amount of CPU time that has been used since this period
   *  was last initiated.  If the period is expired or has not been initiated,
   *  then this field has no meaning.
   */
  struct timespec                      executed_since_last_period;

  /** This is the count of postponed jobs of this period. */
  uint32_t                             postponed_jobs_count;
}  rtems_rate_monotonic_period_status;

/**
 *  @brief Create a Period
 *
 *  Rate Monotonic Manager
 *
 *  This routine implements the rate_monotonic_create directive.  The
 *  period will have the name name.  It returns the id of the
 *  created period in ID.
 */
rtems_status_code rtems_rate_monotonic_create(
  rtems_name    name,
  rtems_id     *id
);

/**
 * @brief RTEMS Rate Monotonic Name to Id
 *
 * This routine implements the rtems_rate_monotonic_ident directive.
 * It returns the period ID associated with name. If more than one period
 * is named name, then the period to which the ID belongs is arbitrary.
 *
 * @param[in] name is the user defined period name
 * @param[in] id is the pointer to period id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the id will
 *         be filled in with the region id.
 */
rtems_status_code rtems_rate_monotonic_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 * @brief RTEMS Rate Monotonic Cancel
 *
 * This routine implements the rtems_rate_monotonic_cancel directive. This
 * directive stops the period associated with ID from continuing to
 * run.
 *
 * @param[in] id is the rate monotonic id
 *
 * @retval RTEMS_SUCCESSFUL if successful and caller is not the owning thread
 * or error code if unsuccessful
 *
 */
rtems_status_code rtems_rate_monotonic_cancel(
  rtems_id   id
);

/**
 * @brief RTEMS Delete Rate Monotonic
 *
 * This routine implements the rtems_rate_monotonic_delete directive. The
 * period indicated by ID is deleted.
 *
 * @param[in] id is the rate monotonic id
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_rate_monotonic_delete(
  rtems_id   id
);

/**
 * @brief RTEMS Rate Monotonic Get Status
 *
 * This routine implements the rtems_rate_monotonic_get_status directive.
 * Information about the period indicated by ID is returned.
 *
 * @param[in] id is the rate monotonic id
 * @param[in] status is the pointer to status control block
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 *
 */
rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                             id,
  rtems_rate_monotonic_period_status  *status
);

/**
 * @brief RTEMS Rate Monotonic Get Statistics
 *
 * This routine implements the rtems_rate_monotonic_get_statistics directive.
 * Statistics gathered from the use of this period are returned.
 *
 * @param[in] id is the rate monotonic id
 * @param[in] statistics is the pointer to statistics control block
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_rate_monotonic_get_statistics(
  rtems_id                                id,
  rtems_rate_monotonic_period_statistics *statistics
);

/**
 *  @brief RTEMS Rate Monotonic Reset Statistics
 *
 *  Rate Monotonic Manager -- Reset Statistics
 *
 *  This routine allows a thread to reset the statistics information
 *  on a specific period instance.
 */
rtems_status_code rtems_rate_monotonic_reset_statistics(
  rtems_id                                 id
);

/**
 *  @brief rtems_rate_monotonic_reset_all_statistics
 *
 *  This routine allows a thread to reset the statistics information
 *  on ALL period instances.
 */
void rtems_rate_monotonic_reset_all_statistics( void );

/**
 *  @brief RTEMS Report Rate Monotonic Statistics
 *
 *  This routine allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using the RTEMS
 *  printer. The implementation of this directive straddles the fence
 *  between inside and outside of RTEMS.  It is presented as part of
 *  the Manager but actually uses other services of the Manager.
 */
void rtems_rate_monotonic_report_statistics_with_plugin(
  const struct rtems_printer *printer
);

/**
 *  @brief RTEMS Report Rate Monotonic Statistics
 *
 *  This routine allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 */
void rtems_rate_monotonic_report_statistics( void );

/**
 * @brief RTEMS Rate Monotonic Period
 *
 * This routine implements the rtems_rate_monotonic_period directive. When
 * length is non-zero, this directive initiates the period associated with
 * ID from continuing for a period of length. If length is zero, then
 * result is set to indicate the current state of the period.
 *
 * @param[in] id is the rate monotonic id
 * @param[in] length is the length of period (in ticks)
 *
 * @retval RTEMS_SUCCESSFUL if successful or error code if unsuccessful
 */
rtems_status_code rtems_rate_monotonic_period(
  rtems_id        id,
  rtems_interval  length
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
