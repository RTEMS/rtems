/*  clock.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Clock Manager.  This manager provides facilities to set, obtain,
 *  and continually update the current date and time.
 *
 *  This manager provides directives to:
 *
 *     + set the current date and time
 *     + obtain the current date and time
 *     + announce a clock tick
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_CLOCK_h
#define __RTEMS_CLOCK_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/tod.h>
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
  unsigned32  seconds;
  unsigned32  microseconds;
} rtems_clock_time_value;

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

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
