/**
 * @file rtems/rtems/ratemon.h
 *
 *  This include file contains all the constants, structures, and
 *  prototypes associated with the Rate Monotonic Manager.  This manager
 *  provides facilities to implement threads which execute in a periodic
 *  fashion.
 *
 *  Directives provided are:
 *
 *     - create a rate monotonic timer
 *     - cancel a period
 *     - delete a rate monotonic timer
 *     - conclude current and start the next period
 *     - obtain status information on a period
 */

/*  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RATEMON_H
#define _RTEMS_RTEMS_RATEMON_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_RATEMON_EXTERN
#define RTEMS_RATEMON_EXTERN extern
#endif

#include <rtems/bspIo.h>

/**
 *  @defgroup ClassicRateMon Rate Monotonic Scheduler
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the
 *  Classic API Rate Monotonic Manager.
 *
 *  Statistics are kept for each period and can be obtained or printed via
 *  API calls.  The statistics kept include minimum, maximum and average times
 *  for both cpu usage and wall time.  The statistics indicate the execution time
 *  used by the owning thread between successive calls to rtems_rate_monotonic_period.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This is the public type used for the rate monotonic timing
 *  statistics.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timespec.h>

  typedef struct timespec rtems_rate_monotonic_period_time_t;
#else
  typedef uint32_t rtems_rate_monotonic_period_time_t;
#endif

/**
 *  This is the internal type used for the rate monotonic timing
 *  statistics.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timestamp.h>

  typedef Timestamp_Control Rate_monotonic_Period_time_t;
#else
  typedef uint32_t Rate_monotonic_Period_time_t;
#endif

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>

#include <string.h>


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
   * the owner is blocked waiting on it.
   */
  RATE_MONOTONIC_OWNER_IS_BLOCKING,

  /**
   * This value indicates the period is on the watchdog chain, and
   * running.  The owner should be executed or blocked waiting on
   * another object.
   */
  RATE_MONOTONIC_ACTIVE,

  /**
   * This value indicates the period is on the watchdog chain, and
   * has expired.  The owner should be blocked waiting for the next period.
   */
  RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING,

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
  rtems_thread_cpu_usage_t             min_cpu_time;
  /** This field contains the highest amount of CPU time used in a period. */
  rtems_thread_cpu_usage_t             max_cpu_time;
  /** This field contains the total amount of wall time used in a period. */
  rtems_thread_cpu_usage_t             total_cpu_time;

  /** This field contains the least amount of wall time used in a period. */
  rtems_rate_monotonic_period_time_t   min_wall_time;
  /** This field contains the highest amount of wall time used in a period. */
  rtems_rate_monotonic_period_time_t   max_wall_time;
  /** This field contains the total amount of CPU time used in a period. */
  rtems_rate_monotonic_period_time_t   total_wall_time;
}  rtems_rate_monotonic_period_statistics;

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
  Thread_CPU_usage_t                   min_cpu_time;
  /** This field contains the highest amount of CPU time used in a period. */
  Thread_CPU_usage_t                   max_cpu_time;
  /** This field contains the total amount of wall time used in a period. */
  Thread_CPU_usage_t                   total_cpu_time;

  /** This field contains the least amount of wall time used in a period. */
  Rate_monotonic_Period_time_t         min_wall_time;
  /** This field contains the highest amount of wall time used in a period. */
  Rate_monotonic_Period_time_t         max_wall_time;
  /** This field contains the total amount of CPU time used in a period. */
  Rate_monotonic_Period_time_t         total_wall_time;
}  Rate_monotonic_Statistics;

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
  rtems_rate_monotonic_period_time_t   since_last_period;

  /**
   *  This is the amount of CPU time that has been used since this period
   *  was last initiated.  If the period is expired or has not been initiated,
   *  then this field has no meaning.
   */
  rtems_thread_cpu_usage_t             executed_since_last_period;
}  rtems_rate_monotonic_period_status;

/**
 *  The following structure defines the control block used to manage
 *  each period.
 */
typedef struct {
  /** This field is the object management portion of a Period instance. */
  Objects_Control                         Object;

  /** This is the timer used to provide the unblocking mechanism. */
  Watchdog_Control                        Timer;

  /** This field indicates the current state of the period. */
  rtems_rate_monotonic_period_states      state;

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
  Thread_CPU_usage_t                      cpu_usage_period_initiated;

  /**
   * This field contains the wall time value when the period
   * was initiated.  It is used to compute the period's statistics.
   */
  Rate_monotonic_Period_time_t            time_period_initiated;

  /**
   * This field contains the statistics maintained for the period.
   */
  Rate_monotonic_Statistics               Statistics;
}   Rate_monotonic_Control;

/**
 *  @brief Rate Monotonic Period Class Management Structure
 *
 *  This instance of Objects_Information is used to manage the
 *  set of rate monotonic period instances.
 */
RTEMS_RATEMON_EXTERN Objects_Information _Rate_monotonic_Information;

/**
 *  @brief Rate Monotonic Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Rate_monotonic_Manager_initialization(void);

/**
 *  @brief rtems_rate_monotonic_create
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
 *  @brief rtems_rate_monotonic_ident
 *
 *  This routine implements the rtems_rate_monotonic_ident directive.
 *  It returns the period ID associated with name.  If more than one period
 *  is named name, then the period to which the ID belongs is arbitrary.
 */
rtems_status_code rtems_rate_monotonic_ident(
  rtems_name    name,
  rtems_id     *id
);

/**
 *  @brief rtems_rate_monotonic_cancel
 *
 *  This routine implements the rtems_rate_monotonic_cancel directive.  This
 *  directive stops the period associated with ID from continuing to
 *  run.
 */
rtems_status_code rtems_rate_monotonic_cancel(
  rtems_id   id
);

/**
 *  @brief rtems_rate_monotonic_delete
 *
 *  This routine implements the rtems_rate_monotonic_delete directive.  The
 *  period indicated by ID is deleted.
 */
rtems_status_code rtems_rate_monotonic_delete(
  rtems_id   id
);

/**
 *  @brief rtems_rate_monotonic_get_status
 *
 *  This routine implements the rtems_rate_monotonic_get_status directive.
 *  Information about the period indicated by ID is returned.
 *
 */
rtems_status_code rtems_rate_monotonic_get_status(
  rtems_id                             id,
  rtems_rate_monotonic_period_status  *status
);

/**
 *  @brief rtems_rate_monotonic_get_statistics
 *
 *  This routine implements the rtems_rate_monotonic_get_statistics directive.
 *  Statistics gathered from the use of this period are returned.
 */
rtems_status_code rtems_rate_monotonic_get_statistics(
  rtems_id                                id,
  rtems_rate_monotonic_period_statistics *statistics
);

/**
 *  @brief rtems_rate_monotonic_reset_statistics
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
 *  @brief rtems_rate_monotonic_report_statistics
 *
 *  This routine allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 */
void rtems_rate_monotonic_report_statistics_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
);

/**
 *  @brief rtems_rate_monotonic_report_statistics
 *
 *  This routine allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 */
void rtems_rate_monotonic_report_statistics( void );

/**
 *  @brief rtems_rate_monotonic_period
 *
 *  This routine implements the rtems_rate_monotonic_period directive.  When
 *  length is non-zero, this directive initiates the period associated with
 *  ID from continuing for a period of length.  If length is zero, then
 *  result is set to indicate the current state of the period.
 */
rtems_status_code rtems_rate_monotonic_period(
  rtems_id        id,
  rtems_interval  length
);

/**
 *  @brief _Rate_monotonic_Timeout
 *
 *  This routine is invoked when the period represented
 *  by ID expires.  If the thread which owns this period is blocked
 *  waiting for the period to expire, then it is readied and the
 *  period is restarted.  If the owning thread is not waiting for the
 *  period to expire, then the period is placed in the EXPIRED
 *  state and not restarted.
 */
void _Rate_monotonic_Timeout(
  rtems_id    id,
  void       *ignored
);

/**
 *  @brief _Rate_monotonic_Get_status(
 *
 *  This routine is invoked to compute the elapsed wall time and cpu
 *  time for a period.
 *
 *  @param[in] the_period points to the period being operated upon.
 *  @param[out] wall_since_last_period is set to the wall time elapsed
 *              since the period was initiated.
 *  @param[out] cpu_since_last_period is set to the cpu time used by the
 *              owning thread since the period was initiated.
 *
 *  @return This routine returns true if the status can be determined
 *          and false otherwise.
 */
bool _Rate_monotonic_Get_status(
  Rate_monotonic_Control        *the_period,
  Rate_monotonic_Period_time_t  *wall_since_last_period,
  Thread_CPU_usage_t            *cpu_since_last_period
);

/**
 *  @brief _Rate_monotonic_Initiate_statistics(
 *
 *  This routine is invoked when a period is initiated via an explicit
 *  call to rtems_rate_monotonic_period for the period's first iteration
 *  or from _Rate_monotonic_Timeout for period iterations 2-n.
 *
 *  @param[in] the_period points to the period being operated upon.
 */
void _Rate_monotonic_Initiate_statistics(
  Rate_monotonic_Control *the_period
);

/**
 *  @brief _Rate_monotonic_Reset_wall_time_statistics
 *
 *  This method resets the statistics information for a period instance.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #define _Rate_monotonic_Reset_wall_time_statistics( _the_period ) \
     do { \
        /* set the minimums to a large value */ \
        _Timestamp_Set( \
          &(_the_period)->Statistics.min_wall_time, \
          0x7fffffff, \
          0x7fffffff \
        ); \
     } while (0)
#else
  #define _Rate_monotonic_Reset_wall_time_statistics( _the_period ) \
     do { \
        /* set the minimum to a large value */ \
        (_the_period)->Statistics.min_wall_time = 0xffffffff; \
     } while (0)
#endif

/**
 *  @brief Rate_monotonic_Reset_cpu_use_statistics
 *
 *  This helper method resets the period CPU usage statistics structure.
 */
#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #define _Rate_monotonic_Reset_cpu_use_statistics( _the_period ) \
     do { \
        /* set the minimums to a large value */ \
        _Timestamp_Set( \
          &(_the_period)->Statistics.min_cpu_time, \
          0x7fffffff, \
          0x7fffffff \
        ); \
     } while (0)
#else
  #define _Rate_monotonic_Reset_cpu_use_statistics( _the_period ) \
     do { \
        /* set the minimum to a large value */ \
        (_the_period)->Statistics.min_cpu_time = 0xffffffff; \
     } while (0)
#endif

/**
 *  @brief Rate_monotonic_Reset_statistics
 *
 *  This helper method resets the period wall time statistics structure.
 */
#define _Rate_monotonic_Reset_statistics( _the_period ) \
  do { \
    memset( \
      &(_the_period)->Statistics, \
      0, \
      sizeof( rtems_rate_monotonic_period_statistics ) \
    ); \
    _Rate_monotonic_Reset_cpu_use_statistics( _the_period ); \
    _Rate_monotonic_Reset_wall_time_statistics( _the_period ); \
  } while (0)

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/ratemon.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
