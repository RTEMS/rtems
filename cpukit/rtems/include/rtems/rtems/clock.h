/**
 * @file rtems/rtems/clock.h
 *
 * @defgroup ClassicClock Clocks
 *
 * @ingroup ClassicRTEMS
 * @brief Clock Manager API
 *
 * This include file contains all the constants and structures associated
 * with the Clock Manager. This manager provides facilities to set, obtain,
 * and continually update the current date and time.
 *
 * This manager provides directives to:
 *
 * - set the current date and time
 * - obtain the current date and time
 * - announce a clock tick
 * - obtain the system uptime
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_CLOCK_H
#define _RTEMS_RTEMS_CLOCK_H

#include <rtems/score/watchdog.h>
#include <rtems/score/tod.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/config.h>
#include <rtems/score/timecounterimpl.h>

#include <sys/time.h> /* struct timeval */

/**
 *  @defgroup ClassicClock Clocks
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the Classic API Clock
 *  Manager.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  List of things which can be returned by the rtems_clock_get directive.
 */
typedef enum {
  /** This value indicates obtain TOD in Classic API format. */
  RTEMS_CLOCK_GET_TOD,
  /** This value indicates obtain the number of seconds since the epoch. */
  RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH,
  /** This value indicates obtain the number of ticks since system boot. */
  RTEMS_CLOCK_GET_TICKS_SINCE_BOOT,
  /** This value indicates obtain the number of ticks per second. */
  RTEMS_CLOCK_GET_TICKS_PER_SECOND,
  /** This value indicates obtain the TOD in struct timeval format. */
  RTEMS_CLOCK_GET_TIME_VALUE
} rtems_clock_get_options;

/**
 * @brief Obtain Current Time of Day
 *
 * @deprecated rtems_clock_get() is deprecated. Use the more explicit
 * function calls rtems_clock_get_xxx().
 *
 * This routine implements the rtems_clock_get directive. It returns
 * one of the following:
 * + current time of day
 * + seconds since epoch
 * + ticks since boot
 * + ticks per second
 *
 * @param[in] option is the format of time to return
 * @param[in] time_buffer points to the output area
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 */
rtems_status_code rtems_clock_get(
  rtems_clock_get_options  option,
  void                    *time_buffer
) RTEMS_COMPILER_DEPRECATED_ATTRIBUTE;

/**
 * @brief Obtain Current Time of Day (Classic TOD)
 *
 * This routine implements the rtems_clock_get_tod directive. It returns
 * the current time of day in the format defined by RTEID.
 *
 * Clock Manager - rtems_clock_get_tod
 *
 * @param[in] time_buffer points to the time of day structure
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the time_buffer will
 *         be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_tod(
  rtems_time_of_day *time_buffer
);

/**
 * @brief Obtain TOD in struct timeval Format
 *
 * This routine implements the rtems_clock_get_tod_timeval
 * directive.
 *
 * @param[in] time points to the struct timeval variable to fill in
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the time will
 *         be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_tod_timeval(
  struct timeval *time
);

/**
 * @brief Obtain Seconds Since Epoch
 *
 * This routine implements the rtems_clock_get_seconds_since_epoch
 * directive.
 *
 * @param[in] the_interval points to the interval variable to fill in
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the time_buffer will
 *         be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_seconds_since_epoch(
  rtems_interval *the_interval
);

/**
 * @brief Gets the current ticks counter value.
 *
 * @return The current tick counter value.  With a 1ms clock tick, this counter
 * overflows after 50 days since boot.
 */
RTEMS_INLINE_ROUTINE rtems_interval rtems_clock_get_ticks_since_boot(void)
{
  return _Watchdog_Ticks_since_boot;
}

/**
 * @brief Returns the ticks counter value delta ticks in the future.
 *
 * @param[in] delta The ticks delta value.
 *
 * @return The tick counter value delta ticks in the future.
 */
RTEMS_INLINE_ROUTINE rtems_interval rtems_clock_tick_later(
  rtems_interval delta
)
{
  return _Watchdog_Ticks_since_boot + delta;
}

/**
 * @brief Returns the ticks counter value at least delta microseconds in the
 * future.
 *
 * @param[in] delta_in_usec The delta value in microseconds.
 *
 * @return The tick counter value at least delta microseconds in the future.
 */
RTEMS_INLINE_ROUTINE rtems_interval rtems_clock_tick_later_usec(
  rtems_interval delta_in_usec
)
{
  rtems_interval us_per_tick = rtems_configuration_get_microseconds_per_tick();

  /*
   * Add one additional tick, since we don't know the time to the clock next
   * tick.
   */
  return _Watchdog_Ticks_since_boot
    + (delta_in_usec + us_per_tick - 1) / us_per_tick + 1;
}

/**
 * @brief Returns true if the current ticks counter value indicates a time
 * before the time specified by the tick value and false otherwise.
 *
 * @param[in] tick The tick value.
 *
 * This can be used to write busy loops with a timeout.
 *
 * @code
 * status busy( void )
 * {
 *   rtems_interval timeout = rtems_clock_tick_later_usec( 10000 );
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
 *
 * @retval true The current ticks counter value indicates a time before the
 * time specified by the tick value.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_clock_tick_before(
  rtems_interval tick
)
{
  return (int32_t) ( tick - _Watchdog_Ticks_since_boot ) > 0;
}

/**
 * @brief Obtain Ticks Per Seconds
 *
 * This routine implements the rtems_clock_get_ticks_per_second
 * directive.
 *
 * @retval This method returns the number of ticks per second. It cannot
 *         fail since RTEMS is always configured to know the number of
 *         ticks per second.
 */
rtems_interval rtems_clock_get_ticks_per_second(void);

/**
 * @brief Set the Current TOD
 *
 * This routine implements the rtems_clock_set directive. It sets
 * the current time of day to that in the time_buffer record.
 *
 * @param[in] time_buffer points to the new TOD
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error.
 *
 * @note Activities scheduled based upon the current time of day
 *       may be executed immediately if the time is moved forward.
 */
rtems_status_code rtems_clock_set(
  const rtems_time_of_day *time_buffer
);

/**
 * @brief Announce a Clock Tick
 *
 * This routine implements the rtems_clock_tick directive. It is invoked
 * to inform RTEMS of the occurrence of a clock tick.
 *
 * @retval This directive always returns RTEMS_SUCCESSFUL.
 *
 * @note This method is typically called from an ISR and is the basis
 *       for all timeouts and delays. This routine only works for leap-years
 *       through 2099.
 */
rtems_status_code rtems_clock_tick( void );

/**
 * @brief Obtain the System Uptime
 *
 * This directive returns the system uptime.
 *
 * @param[in] uptime is a pointer to the time structure
 *
 * @retval This method returns RTEMS_SUCCESSFUL if there was not an
 *         error. Otherwise, a status code is returned indicating the
 *         source of the error. If successful, the @a uptime will be
 *         filled in.
 */
rtems_status_code rtems_clock_get_uptime(
  struct timespec *uptime
);

/**
 * @brief Gets the System Uptime in the Struct Timeval Format
 *
 * @param[out] uptime is a pointer to a struct timeval structure.
 *
 * @retval This methods returns the system uptime.
 *
 * @note Pointer must not be NULL.
 */
void rtems_clock_get_uptime_timeval( struct timeval *uptime );

/**
 * @brief Returns the system uptime in seconds.
 *
 * @retval The system uptime in seconds.
 */
RTEMS_INLINE_ROUTINE time_t rtems_clock_get_uptime_seconds( void )
{
  return _Timecounter_Time_uptime - 1;
}

/**
 * @brief Returns the system uptime in nanoseconds.
 *
 * @retval The system uptime in nanoseconds.
 */
uint64_t rtems_clock_get_uptime_nanoseconds( void );

/**
 * @brief TOD Validate
 *
 * This support function returns true if @a the_tod contains
 * a valid time of day, and false otherwise.
 *
 * @param[in] the_tod is the TOD structure to validate
 *
 * @retval This method returns true if the TOD is valid and false otherwise.
 *
 * @note This routine only works for leap-years through 2099.
 */
bool _TOD_Validate(
  const rtems_time_of_day *the_tod
);

/**
 * @brief TOD to Seconds
 *
 * This function returns the number seconds between the epoch and @a the_tod.
 *
 * @param[in] the_tod is the TOD structure to convert to seconds
 *
 * @retval This method returns the number of seconds since epoch represented
 *         by @a the_tod
 */
Watchdog_Interval _TOD_To_seconds(
  const rtems_time_of_day *the_tod
);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
