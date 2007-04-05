/** 
 *  @file  rtems/score/tod.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Time of Day Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_TOD_H
#define _RTEMS_SCORE_TOD_H

/**
 *  @defgroup ScoreTOD Time Of Day (TOD) Handler
 *
 *  This handler encapsulates functionality used to manage time of day. 
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/watchdog.h>
#include <time.h>

/** @defgroup ScoreTODConstants TOD Constants
 *  The following constants are related to the time of day.
 */
/**@{*/

/**
 *  This constant represents the number of seconds in a minute.
 */
#define TOD_SECONDS_PER_MINUTE (uint32_t)60

/**
 *  This constant represents the number of minutes per hour.
 */
#define TOD_MINUTES_PER_HOUR   (uint32_t)60

/**
 *  This constant represents the number of months in a year.
 */
#define TOD_MONTHS_PER_YEAR    (uint32_t)12

/**
 *  This constant represents the number of days in a non-leap year.
 */
#define TOD_DAYS_PER_YEAR      (uint32_t)365

/**
 *  This constant represents the number of hours per day.
 */
#define TOD_HOURS_PER_DAY      (uint32_t)24

/**
 *  This constant represents the number of seconds in a day which does
 *  not include a leap second.
 */
#define TOD_SECONDS_PER_DAY    (uint32_t) (TOD_SECONDS_PER_MINUTE * \
                                TOD_MINUTES_PER_HOUR   * \
                                TOD_HOURS_PER_DAY)

/**
 *  This constant represents the number of seconds in a non-leap year.
 */
#define TOD_SECONDS_PER_NON_LEAP_YEAR (365 * TOD_SECONDS_PER_DAY)

/**
 *  This constant represents the number of seconds in a millisecond.
 */
#define TOD_MILLISECONDS_PER_SECOND     (uint32_t)1000

/**
 *  This constant represents the number of microseconds in a second.
 */
#define TOD_MICROSECONDS_PER_SECOND     (uint32_t)1000000

/**
 *  This constant represents the number of nanoseconds in a second.
 */
#define TOD_NANOSECONDS_PER_SECOND      (uint32_t)1000000000

/**
 *  This constant represents the number of nanoseconds in a second.
 */
#define TOD_NANOSECONDS_PER_MICROSECOND (uint32_t)1000

/*
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core. The timespec format time
 *  is kept in POSIX compliant form.
 */
#define TOD_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))

/**@}*/

/** @brief RTEMS Epoch Year
 *
 *  The following constant define the earliest year to which an
 *  time of day can be initialized.  This is considered the
 *  epoch.
 */
#define TOD_BASE_YEAR 1988

/** @brief Is the Time Of Day Set
 *
 *  This is TRUE if the application has set the current
 *  time of day, and FALSE otherwise.
 */
SCORE_EXTERN boolean _TOD_Is_set;

/** @brief Current Time of Day (Timespec)
 *  The following contains the current time of day.
 */
SCORE_EXTERN struct timespec _TOD_Now;

/** @brief Current Time of Day (Timespec)
 *  The following contains the running uptime.
 */
SCORE_EXTERN struct timespec _TOD_Uptime;

/** @brief Seconds Since RTEMS Epoch
 *  The following contains the number of seconds from 00:00:00
 *  January 1, TOD_BASE_YEAR until the current time of day.
 */
#define _TOD_Seconds_since_epoch (_TOD_Now.tv_sec)

/** @brief Microseconds per Clock Tick
 *
 *  The following contains the number of microseconds per tick.
 */
SCORE_EXTERN uint32_t   _TOD_Microseconds_per_tick;

/** @brief _TOD_Handler_initialization
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _TOD_Handler_initialization(
  uint32_t   microseconds_per_tick
);

/** @brief _TOD_Set
 *
 *  This routine sets the current time of day to @a time and
 *  the equivalent SECONDS_SINCE_EPOCH.
 */
void _TOD_Set(
  const struct timespec *time
);

/** @brief _TOD_Get
 *
 *  This routine returns the current time of day with potential accuracy
 *  to the nanosecond.  
 *
 *  @param[in] time is a pointer to the time to be returned
 */
void _TOD_Get(
  struct timespec *time
);

/** @brief _TOD_Get_uptime
 *
 *  This routine returns the system uptime with potential accuracy
 *  to the nanosecond.  
 *
 *  @param[in] time is a pointer to the uptime to be returned
 */
void _TOD_Get_uptime(
  struct timespec *time
);

/**
 *  This routine increments the ticks field of the current time of
 *  day at each clock tick.
 */
void _TOD_Tickle_ticks( void );

/** @brief TOD_MILLISECONDS_TO_MICROSECONDS
 *
 *  This routine converts an interval expressed in milliseconds to microseconds.
 *
 *  @note This must be a macro so it can be used in "static" tables.
 */
#define TOD_MILLISECONDS_TO_MICROSECONDS(_ms) ((uint32_t)(_ms) * 1000L)

/** @brief TOD_MICROSECONDS_TO_TICKS
 *
 *  This routine converts an interval expressed in microseconds to ticks.
 *
 *  @note This must be a macro so it can be used in "static" tables.
 */
#define TOD_MICROSECONDS_TO_TICKS(_us) \
    ((_us) / _TOD_Microseconds_per_tick)

/** @brief TOD_MILLISECONDS_TO_TICKS
 *
 *  This routine converts an interval expressed in milliseconds to ticks.
 *
 *  @note This must be a macro so it can be used in "static" tables.
 */

#define TOD_MILLISECONDS_TO_TICKS(_ms) \
    (TOD_MILLISECONDS_TO_MICROSECONDS(_ms) / _TOD_Microseconds_per_tick)


/** @brief How many ticks in a second?
 *
 *  This macro returns the number of ticks in a second.
 *
 *  @note If the clock tick value does not multiply evenly into a second
 *        then this number of ticks will be slightly shorter than a second.
 */
#define TOD_TICKS_PER_SECOND \
  (TOD_MICROSECONDS_PER_SECOND / _TOD_Microseconds_per_tick)

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tod.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
