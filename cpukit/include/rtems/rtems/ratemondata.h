/**
 * @file
 *
 * @ingroup ClassicRateMonImpl
 *
 * @brief Classic Rate Monotonic Scheduler Data Structures
 */

/* COPYRIGHT (c) 1989-2009, 2016.
 * On-Line Applications Research Corporation (OAR).
 * COPYRIGHT (c) 2016-2017 Kuan-Hsun Chen.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RATEMONDATA_H
#define _RTEMS_RTEMS_RATEMONDATA_H

#include <rtems/rtems/ratemon.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicRateMonImpl
 *
 * @{
 */

/**
 *  The following defines the INTERNAL data structure that has the
 *  statistics kept on each period instance.
 */
typedef struct {
  /** This field contains the number of periods executed. */
  uint32_t     count;
  /** This field contains the number of periods missed. */
  uint32_t     missed_count;

  /** This field contains the least amount of CPU time used in a period. */
  Timestamp_Control min_cpu_time;
  /** This field contains the highest amount of CPU time used in a period. */
  Timestamp_Control max_cpu_time;
  /** This field contains the total amount of wall time used in a period. */
  Timestamp_Control total_cpu_time;

  /** This field contains the least amount of wall time used in a period. */
  Timestamp_Control min_wall_time;
  /** This field contains the highest amount of wall time used in a period. */
  Timestamp_Control max_wall_time;
  /** This field contains the total amount of CPU time used in a period. */
  Timestamp_Control total_wall_time;
}  Rate_monotonic_Statistics;

/**
 * @brief The following structure defines the control block used to manage each
 * period.
 *
 * State changes are protected by the default thread lock of the owner thread.
 * The owner thread is the thread that created the period object.  The owner
 * thread field is immutable after object creation.
 */
typedef struct {
  /** This field is the object management portion of a Period instance. */
  Objects_Control                         Object;

  /**
   * @brief Protects the rate monotonic period state.
   */
  ISR_LOCK_MEMBER(                        Lock )

  /** This is the timer used to provide the unblocking mechanism. */
  Watchdog_Control                        Timer;

  /** This field indicates the current state of the period. */
  rtems_rate_monotonic_period_states      state;

  /**
   * @brief A priority node for use by the scheduler job release and cancel
   * operations.
   */
  Priority_Node                           Priority;

  /**
   * This field contains the length of the next period to be
   * executed.
   */
  uint32_t                                next_length;

  /**
   * This field contains a pointer to the TCB for the thread
   * which owns and uses this period instance.
   */
  Thread_Control                         *owner;

  /**
   * This field contains the cpu usage value of the owning thread when
   * the period was initiated.  It is used to compute the period's
   * statistics.
   */
  Timestamp_Control                       cpu_usage_period_initiated;

  /**
   * This field contains the wall time value when the period
   * was initiated.  It is used to compute the period's statistics.
   */
  Timestamp_Control                       time_period_initiated;

  /**
   * This field contains the statistics maintained for the period.
   */
  Rate_monotonic_Statistics               Statistics;

  /**
   * This field contains the number of postponed jobs.
   * When the watchdog timeout, this variable will be increased immediately.
   */
  uint32_t                                postponed_jobs;

  /**
   *  This field contains the tick of the latest deadline decided by the period
   *  watchdog.
  */
  uint64_t                                latest_deadline;
}   Rate_monotonic_Control;

/**
 * @brief The Classic Rate Monotonic objects information.
 */
extern Objects_Information _Rate_monotonic_Information;

/**
 * @brief Macro to define the objects information for the Classic Rate
 * Monotonic objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define RATE_MONOTONIC_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Rate_monotonic, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_PERIODS, \
    Rate_monotonic_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
