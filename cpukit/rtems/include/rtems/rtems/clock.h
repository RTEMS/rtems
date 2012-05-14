/**
 * @file rtems/rtems/clock.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Clock Manager.  This manager provides facilities to set, obtain,
 *  and continually update the current date and time.
 *
 *  This manager provides directives to:
 *
 *     - set the current date and time
 *     - obtain the current date and time
 *     - set the nanoseconds since last clock tick handler
 *     - announce a clock tick
 *     - obtain the system uptime
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_CLOCK_H
#define _RTEMS_RTEMS_CLOCK_H

#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <sys/time.h> /* struct timeval */

/**
 *  @defgroup ClassicClock Clocks
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
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
 *  Type for the nanoseconds since last tick BSP extension.
 */
typedef Watchdog_Nanoseconds_since_last_tick_routine
  rtems_nanoseconds_extension_routine;

/**
 *  @brief Obtain Current Time of Day
 *
 *  This routine implements the rtems_clock_get directive.  It returns
 *  one of the following:
 *    + current time of day
 *    + seconds since epoch
 *    + ticks since boot
 *    + ticks per second
 *
 *  @param[in] option is the format of time to return
 *  @param[in] time_buffer points to the output area
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.
 */
rtems_status_code rtems_clock_get(
  rtems_clock_get_options  option,
  void                    *time_buffer
);

/**
 *  @brief Obtain Current Time of Day (Classic TOD)
 *
 *  This routine implements the rtems_clock_get_tod directive.  It returns
 *  the current time of day in the format defined by RTEID.
 *
 *  @param[in] time_buffer points to the time of day structure
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the time_buffer will
 *          be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_tod(
  rtems_time_of_day *time_buffer
);

/**
 *  @brief Obtain TOD in struct timeval Format
 *
 *  This routine implements the rtems_clock_get_tod_timeval
 *  directive.
 *
 *  @param[in] time points to the struct timeval variable to fill in
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the time will
 *          be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_tod_timeval(
  struct timeval *time
);

/**
 *  @brief Obtain Seconds Since Epoch
 *
 *  This routine implements the rtems_clock_get_seconds_since_epoch
 *  directive.
 *
 *  @param[in] the_interval points to the interval variable to fill in
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the time_buffer will
 *          be filled in with the current time of day.
 */
rtems_status_code rtems_clock_get_seconds_since_epoch(
  rtems_interval *the_interval
);

/**
 *  @brief Obtain Ticks Since Boot
 *
 *  This routine implements the rtems_clock_get_ticks_since_boot
 *  directive.
 *
 *  @return This method returns the number of ticks since boot.  It cannot
 *          fail since RTEMS always keeps a running count of ticks since boot.
 */
rtems_interval rtems_clock_get_ticks_since_boot(void);

/**
 *  @brief Obtain Ticks Per Seconds
 *
 *  This routine implements the rtems_clock_get_ticks_per_second
 *  directive.
 *
 *  @return This method returns the number of ticks since boot.  It cannot
 *          fail since RTEMS is always configured to know the number of
 *          ticks per second.
 */
rtems_interval rtems_clock_get_ticks_per_second(void);

/**
 *  @brief Set the Current TOD
 *
 *  This routine implements the rtems_clock_set directive.  It sets
 *  the current time of day to that in the time_buffer record.
 *
 *  @param[in] time_buffer points to the new TOD
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.
 *
 *  @note Activities scheduled based upon the current time of day
 *        may be executed immediately if the time is moved forward.
 */
rtems_status_code rtems_clock_set(
  const rtems_time_of_day *time_buffer
);

/**
 *  @brief Announce a Clock Tick
 *
 *  This routine implements the rtems_clock_tick directive.  It is invoked
 *  to inform RTEMS of the occurrence of a clock tick.
 *
 *  @return This directive always returns RTEMS_SUCCESSFUL.
 *
 *  @note This method is typically called from an ISR and is the basis
 *        for all timeouts and delays.
 */
rtems_status_code rtems_clock_tick( void );

/**
 *  @brief Set the BSP specific Nanoseconds Extension
 *
 *  This directive sets the BSP provided nanoseconds since last tick
 *  extension.
 *
 *  @param[in] routine is a pointer to the extension routine
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.
 */
rtems_status_code rtems_clock_set_nanoseconds_extension(
  rtems_nanoseconds_extension_routine routine
);

/**
 *  @brief Obtain the System Uptime
 *
 *  This directive returns the system uptime.
 *
 *  @param[in] uptime is a pointer to the time structure
 *
 *  @return This method returns RTEMS_SUCCESSFUL if there was not an
 *          error.  Otherwise, a status code is returned indicating the
 *          source of the error.  If successful, the uptime will be
 *          filled in.
 */
rtems_status_code rtems_clock_get_uptime(
  struct timespec *uptime
);

/**
 *  @brief _TOD_Validate
 *
 *  This support function returns true if @a the_tod contains
 *  a valid time of day, and false otherwise.
 *
 *  @param[in] the_tod is the TOD structure to validate
 *
 *  @return This method returns true if the TOD is valid and false otherwise.
 */
bool _TOD_Validate(
  const rtems_time_of_day *the_tod
);

/**
 *  @brief _TOD_To_seconds
 *
 *  This function returns the number seconds between the epoch and @a the_tod.
 *
 *  @param[in] the_tod is the TOD structure to convert to seconds
 *
 *  @return This method returns the number of seconds since epoch represented
 *          by @a the_tod
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
