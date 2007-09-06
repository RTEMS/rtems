/**
 * @file rtems/rtems/ratemon.h
 */

/*
 *  This include file contains all the constants, structures, and
 *  prototypes associated with the Rate Monotonic Manager.  This manager
 *  provides facilities to implement tasks which execute in a periodic fashion.
 *
 *  Directives provided are:
 *
 *     + create a rate monotonic timer
 *     + cancel a period
 *     + delete a rate monotonic timer
 *     + conclude current and start the next period
 *     + obtain status information on a period
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_RTEMS_RATEMON_H
#define _RTEMS_RTEMS_RATEMON_H

#ifndef RTEMS_RATEMON_EXTERN
#define RTEMS_RATEMON_EXTERN extern
#endif

#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The user can define this at configure time and go back to ticks
 *  resolution.
 */
#ifndef __RTEMS_USE_TICKS_RATE_MONOTONIC_STATISTICS__
  /*
   *  Enable the nanosecond accurate statistics
   *
   *  When not defined, the older style tick accurate granularity
   *  is used.
   */
  #define RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
#endif

#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>

#include <string.h>

/*
 *  The following enumerated type defines the states in which a
 *  period may be.
 */

typedef enum {
  RATE_MONOTONIC_INACTIVE,               /* off chain, never initialized */
  RATE_MONOTONIC_OWNER_IS_BLOCKING,      /* on chain, owner is blocking on it */
  RATE_MONOTONIC_ACTIVE,                 /* on chain, running continuously */
  RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING, /* on chain, expired while owner was */
                                         /*   was blocking on it */
  RATE_MONOTONIC_EXPIRED                 /* off chain, will be reset by next */
                                         /*   rtems_rate_monotonic_period */
}   rtems_rate_monotonic_period_states;

/*
 *  The following constant is the interval passed to the rate_monontonic_period
 *  directive to obtain status information.
 */

#define RTEMS_PERIOD_STATUS       WATCHDOG_NO_TIMEOUT

/*
 *  The following defines the statistics kept on each period instance.
 */

typedef struct {
  uint32_t     count;
  uint32_t     missed_count;
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    struct timespec min_cpu_time;
    struct timespec max_cpu_time;
    struct timespec total_cpu_time;
  #else
    uint32_t  min_cpu_time;
    uint32_t  max_cpu_time;
    uint32_t  total_cpu_time;
  #endif
  #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    struct timespec min_wall_time;
    struct timespec max_wall_time;
    struct timespec total_wall_time;
  #else
    uint32_t  min_wall_time;
    uint32_t  max_wall_time;
    uint32_t  total_wall_time;
  #endif
}  rtems_rate_monotonic_period_statistics;

/*
 *  The following defines the period status structure.
 */

typedef struct {
  Objects_Id                          owner;
  rtems_rate_monotonic_period_states  state;
  #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    struct timespec                   since_last_period;
  #else
    uint32_t                          ticks_since_last_period;
  #endif
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    struct timespec                   executed_since_last_period;
  #else
    uint32_t                          ticks_executed_since_last_period;
  #endif
}  rtems_rate_monotonic_period_status;

/*
 *  The following structure defines the control block used to manage
 *  each period.
 */

typedef struct {
  Objects_Control                         Object;
  Watchdog_Control                        Timer;
  rtems_rate_monotonic_period_states      state;
  #ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
    struct timespec                       owner_executed_at_period;
  #else
    uint32_t                              owner_ticks_executed_at_period;
  #endif
  #ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
    struct timespec                       time_at_period;
  #else
    uint32_t                              time_at_period;
  #endif
  uint32_t                                next_length;
  Thread_Control                         *owner;
  rtems_rate_monotonic_period_statistics  Statistics;
}   Rate_monotonic_Control;

RTEMS_RATEMON_EXTERN Objects_Information _Rate_monotonic_Information;

/*
 *  _Rate_monotonic_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Rate_monotonic_Manager_initialization(
  uint32_t   maximum_periods
);

/*
 *  rtems_rate_monotonic_create
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rate_monotonic_create directive.  The
 *  period will have the name name.  It returns the id of the
 *  created period in ID.
 */

rtems_status_code rtems_rate_monotonic_create(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_rate_monotonic_ident
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_ident directive.
 *  This directive returns the period ID associated with name.
 *  If more than one period is named name, then the period
 *  to which the ID belongs is arbitrary.
 */

rtems_status_code rtems_rate_monotonic_ident(
  rtems_name    name,
  Objects_Id   *id
);

/*
 *  rtems_rate_monotonic_cancel
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_cancel directive.  This
 *  directive stops the period associated with ID from continuing to
 *  run.
 */

rtems_status_code rtems_rate_monotonic_cancel(
  Objects_Id id
);

/*
 *  rtems_rate_monotonic_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_delete directive.  The
 *  period indicated by ID is deleted.
 */

rtems_status_code rtems_rate_monotonic_delete(
  Objects_Id id
);

/*
 *  rtems_rate_monotonic_get_status
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_get_status directive.
 *  Information about the period indicated by ID is returned.
 *
 */

rtems_status_code rtems_rate_monotonic_get_status(
  Objects_Id                           id,
  rtems_rate_monotonic_period_status  *status
);

/*
 *  rtems_rate_monotonic_get_statistics
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_get_statistics directive.
 *  Statistics gathered from the use of this period are returned.
 */

rtems_status_code rtems_rate_monotonic_get_statistics(
  Objects_Id                              id,
  rtems_rate_monotonic_period_statistics *statistics
);

/*
 *  rtems_rate_monotonic_reset_statistics
 *
 *  DESCRIPTION:
 *
 *  This directive allows a thread to reset the statistics information
 *  on a specific period instance.
 */
rtems_status_code rtems_rate_monotonic_reset_statistics(
  Objects_Id                               id
);

/*
 *  rtems_rate_monotonic_reset_all_statistics
 *
 *  DESCRIPTION:
 *
 *  This directive allows a thread to reset the statistics information
 *  on ALL period instances.
 */
void rtems_rate_monotonic_reset_all_statistics( void );

/*
 *  rtems_rate_monotonic_report_statistics
 *
 *  DESCRIPTION:
 *
 *  This directive allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 */
void rtems_rate_monotonic_report_statistics_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
);

/*
 *  rtems_rate_monotonic_report_statistics
 *
 *  DESCRIPTION:
 *
 *  This directive allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 */
void rtems_rate_monotonic_report_statistics( void );

/*
 *  rtems_rate_monotonic_period
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_rate_monotonic_period directive.  When
 *  length is non-zero, this directive initiates the period associated with
 *  ID from continuing for a period of length.  If length is zero, then
 *  result is set to indicate the current state of the period.
 */

rtems_status_code rtems_rate_monotonic_period(
  Objects_Id      id,
  rtems_interval  length
);

/*
 *  _Rate_monotonic_Timeout
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when the period represented
 *  by ID expires.  If the task which owns this period is blocked
 *  waiting for the period to expire, then it is readied and the
 *  period is restarted.  If the owning task is not waiting for the
 *  period to expire, then the period is placed in the EXPIRED
 *  state and not restarted.
 */

void _Rate_monotonic_Timeout(
  Objects_Id  id,
  void       *ignored
);

/*
 *  _Rate_monotonic_Reset_statistics
 *
 *  DESCRIPTION:
 *
 *  This method resets the statistics information for a period instance.
 */

#ifdef RTEMS_ENABLE_NANOSECOND_RATE_MONOTONIC_STATISTICS
  #define _Rate_monotonic_Reset_wall_time_statistics( _the_period ) \
     do { \
        /* set the minimums to a large value */ \
        (_the_period)->Statistics.min_wall_time.tv_sec = 0x7fffffff; \
        (_the_period)->Statistics.min_wall_time.tv_nsec = 0x7fffffff; \
     } while (0)
#else
  #define _Rate_monotonic_Reset_wall_time_statistics( _the_period )
#endif

#ifdef RTEMS_ENABLE_NANOSECOND_CPU_USAGE_STATISTICS
  #define _Rate_monotonic_Reset_cpu_use_statistics( _the_period ) \
     do { \
        /* set the minimums to a large value */ \
        (_the_period)->Statistics.min_cpu_time.tv_sec = 0x7fffffff; \
        (_the_period)->Statistics.min_cpu_time.tv_nsec = 0x7fffffff; \
     } while (0)
#else
  #define _Rate_monotonic_Reset_cpu_use_statistics( _the_period )
#endif

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

#endif
/* end of include file */
