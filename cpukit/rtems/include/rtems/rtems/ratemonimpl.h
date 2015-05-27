/**
 * @file
 *
 * @ingroup ClassicRateMonImpl
 *
 * @brief Classic Rate Monotonic Scheduler Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_RATEMONIMPL_H
#define _RTEMS_RTEMS_RATEMONIMPL_H

#include <rtems/rtems/ratemon.h>
#include <rtems/score/objectimpl.h>

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicRateMonImpl Classic Rate Monotonic Scheduler Implementation
 *
 * @ingroup ClassicRateMon
 *
 * @{
 */

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_RATEMON_EXTERN
#define RTEMS_RATEMON_EXTERN extern
#endif

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
 *
 *  @note The Rate Monotonic Manager is built on top of the Watchdog Handler
 */
void _Rate_monotonic_Manager_initialization(void);

/**
 *  @brief Allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  This function allocates a period control block from
 *  the inactive chain of free period control blocks.
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Allocate( void )
{
  return (Rate_monotonic_Control *)
    _Objects_Allocate( &_Rate_monotonic_Information );
}

/**
 *  @brief Allocates a period control block from
 *  the inactive chain of free period control blocks.
 *
 *  This routine allocates a period control block from
 *  the inactive chain of free period control blocks.
 */
RTEMS_INLINE_ROUTINE void _Rate_monotonic_Free (
  Rate_monotonic_Control *the_period
)
{
  _Objects_Free( &_Rate_monotonic_Information, &the_period->Object );
}

/**
 *  @brief Maps period IDs to period control blocks.
 *
 *  This function maps period IDs to period control blocks.
 *  If ID corresponds to a local period, then it returns
 *  the_period control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_period is undefined.
 */
RTEMS_INLINE_ROUTINE Rate_monotonic_Control *_Rate_monotonic_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Rate_monotonic_Control *)
    _Objects_Get( &_Rate_monotonic_Information, id, location );
}

/**
 *  @brief Checks if the_period is in the ACTIVE state.
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_active (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_ACTIVE);
}

/**
 *  @brief Checks if the_period is in the ACTIVE state.
 *
 *  This function returns TRUE if the_period is in the ACTIVE state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_inactive (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_INACTIVE);
}

/**
 *  @brief Checks if the_period is in the EXPIRED state.
 *
 *  This function returns TRUE if the_period is in the EXPIRED state,
 *  and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Rate_monotonic_Is_expired (
  Rate_monotonic_Control *the_period
)
{
  return (the_period->state == RATE_MONOTONIC_EXPIRED);
}

/**
 * @brief Rate Monotonic Timeout
 *
 * This routine is invoked when the period represented
 * by ID expires. If the thread which owns this period is blocked
 * waiting for the period to expire, then it is readied and the
 * period is restarted. If the owning thread is not waiting for the
 * period to expire, then the period is placed in the EXPIRED
 * state and not restarted.
 *
 * @param[in] id is the period id
 */
void _Rate_monotonic_Timeout(
  rtems_id    id,
  void       *ignored
);

/**
 * @brief _Rate_monotonic_Get_status(
 *
 * This routine is invoked to compute the elapsed wall time and cpu
 * time for a period.
 *
 * @param[in] the_period points to the period being operated upon.
 * @param[out] wall_since_last_period is set to the wall time elapsed
 *             since the period was initiated.
 * @param[out] cpu_since_last_period is set to the cpu time used by the
 *             owning thread since the period was initiated.
 *
 * @retval This routine returns true if the status can be determined
 *         and false otherwise.
 */
bool _Rate_monotonic_Get_status(
  Rate_monotonic_Control        *the_period,
  Rate_monotonic_Period_time_t  *wall_since_last_period,
  Thread_CPU_usage_t            *cpu_since_last_period
);

/**
 *  @brief Initiate Rate Monotonic Statistics
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
#define _Rate_monotonic_Reset_wall_time_statistics( _the_period ) \
   do { \
      /* set the minimums to a large value */ \
      _Timestamp_Set( \
	&(_the_period)->Statistics.min_wall_time, \
	0x7fffffff, \
	0x7fffffff \
      ); \
   } while (0)

/**
 *  @brief Rate_monotonic_Reset_cpu_use_statistics
 *
 *  This helper method resets the period CPU usage statistics structure.
 */
#define _Rate_monotonic_Reset_cpu_use_statistics( _the_period ) \
   do { \
      /* set the minimums to a large value */ \
      _Timestamp_Set( \
	&(_the_period)->Statistics.min_cpu_time, \
	0x7fffffff, \
	0x7fffffff \
      ); \
   } while (0)

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

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
