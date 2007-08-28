/**
 * @file rtems/rtems/clock.h
 */

/*
 *  This include file contains all the constants and structures associated
 *  with the Clock Manager.  This manager provides facilities to set, obtain,
 *  and continually update the current date and time.
 *
 *  This manager provides directives to:
 *
 *     + set the current date and time
 *     + obtain the current date and time
 *     + set the nanoseconds since last clock tick handler
 *     + announce a clock tick
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

#ifndef _RTEMS_RTEMS_CLOCK_H
#define _RTEMS_RTEMS_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

/*
 *  List of things which can be returned by the rtems_clock_get directive.
 */

typedef enum {
  RTEMS_CLOCK_GET_TOD,
  RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH,
  RTEMS_CLOCK_GET_TICKS_SINCE_BOOT,
  RTEMS_CLOCK_GET_TICKS_PER_SECOND,
  RTEMS_CLOCK_GET_TIME_VALUE
} rtems_clock_get_options;

/*
 *  Standard flavor style to return TOD in for a rtems_clock_get option.
 */

typedef struct {
  uint32_t    seconds;
  uint32_t    microseconds;
} rtems_clock_time_value;

/*
 *  Type for the nanoseconds since last tick BSP extension.
 */
typedef Watchdog_Nanoseconds_since_last_tick_routine
  rtems_nanoseconds_extension_routine;

/*
 *  rtems_clock_get
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_clock_get directive.  It returns
 *  one of the following:
 *    + current time of day
 *    + seconds since epoch
 *    + ticks since boot
 *    + ticks per second
 */

rtems_status_code rtems_clock_get(
  rtems_clock_get_options  option,
  void              *time_buffer
);

/*
 *  rtems_clock_set
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_clock_set directive.  It sets
 *  the current time of day to that in the time_buffer record.
 */

rtems_status_code rtems_clock_set(
  rtems_time_of_day *time_buffer
);

/*
 *  rtems_clock_tick
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_clock_tick directive.  It is invoked
 *  to inform RTEMS of the occurrence of a clock tick.
 */

rtems_status_code rtems_clock_tick( void );

/*
 *  rtems_clock_set_nanoseconds_extension
 *
 *  DESCRIPTION:
 *
 *  This directive sets the BSP provided nanoseconds since last tick
 *  extension.
 *
 *  Input parameters:
 *    routine - pointer to the extension routine
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */
rtems_status_code rtems_clock_set_nanoseconds_extension(
  rtems_nanoseconds_extension_routine routine
);

/*
 *  rtems_clock_get_uptime
 *
 *  DESCRIPTION:
 *
 *  This directive returns the system uptime.
 *
 *  Input parameters:
 *    routine - pointer to the time structure 
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */
rtems_status_code rtems_clock_get_uptime(
  struct timespec *uptime
);

/** @brief _TOD_Validate
 *
 *  This function returns TRUE if THE_TOD contains
 *  a valid time of day, and FALSE otherwise.
 */
boolean _TOD_Validate(
  rtems_time_of_day *the_tod
);

/** @brief _TOD_To_seconds
 *
 *  This function returns the number seconds between the epoch and THE_TOD.
 */
Watchdog_Interval _TOD_To_seconds(
  rtems_time_of_day *the_tod
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
