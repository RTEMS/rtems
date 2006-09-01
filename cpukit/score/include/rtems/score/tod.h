/** 
 *  @file  rtems/score/tod.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Time of Day Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
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

/**@}*/

/** @brief RTEMS Epoch Year
 *
 *  The following constant define the earliest year to which an
 *  time of day can be initialized.  This is considered the
 *  epoch.
 */
#define TOD_BASE_YEAR 1988

/**
 *  The following record defines the time of control block.  This
 *  control block is used to maintain the current time of day.
 *
 *  @note This is an RTEID style time/date.
 */
typedef struct {
  /** This field is the year, A.D. */
  uint32_t   year;
  /** This field is the month, 1 -> 12 */
  uint32_t   month;
  /** This field is the day, 1 -> 31 */
  uint32_t   day;
  /** This field is the hour, 0 -> 23 */
  uint32_t   hour;
  /** This field is the minute, 0 -> 59 */
  uint32_t   minute;
  /** This field is the second, 0 -> 59 */
  uint32_t   second;
  /** This field is the elapsed ticks between secs */
  uint32_t   ticks;
}   TOD_Control;

/** @brief Is the Time Of Day Set
 *
 *  This is TRUE if the application has set the current
 *  time of day, and FALSE otherwise.
 */
SCORE_EXTERN boolean _TOD_Is_set;

/** @brief Current Time of Day
 *  The following contains the current time of day.
 */
SCORE_EXTERN TOD_Control _TOD_Current;

/** @brief Seconds Since RTEMS Epoch
 *  The following contains the number of seconds from 00:00:00
 *  January 1, TOD_BASE_YEAR until the current time of day.
 */
SCORE_EXTERN Watchdog_Interval _TOD_Seconds_since_epoch;

/** @brief Microseconds per Clock Tick
 *
 *  The following contains the number of microseconds per tick.
 */
SCORE_EXTERN uint32_t   _TOD_Microseconds_per_tick;

/** @brief Clock Ticks per Second
 *
 *  The following contains the number of clock ticks per second.
 *
 *  @note If one second is NOT evenly divisible by the number of microseconds
 *  per clock tick, this value will contain only the integer portion
 *  of the division.  This means that the interval between clock ticks
 *  can be a source of error in the current time of day.
 */
SCORE_EXTERN uint32_t   _TOD_Ticks_per_second;

/** @brief Watchdog Set Managed by Seconds
 *
 *  This is the control structure for the watchdog timer which
 *  fires to service the seconds chain.
 */
SCORE_EXTERN Watchdog_Control _TOD_Seconds_watchdog;

#ifdef SCORE_INIT

/*
 *  The following array contains the number of days in all months.
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 *  The second dimension should range from 1 to 12 for January to
 *  February, respectively.
 */
const uint32_t   _TOD_Days_per_month[ 2 ][ 13 ] = {
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/*
 *  The following array contains the number of days in all months
 *  up to the month indicated by the index of the second dimension.
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 */
const uint16_t   _TOD_Days_to_date[2][13] = {
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

/*
 *  The following array contains the number of days in the years
 *  since the last leap year.  The index should be 0 for leap
 *  years, and the number of years since the beginning of a leap
 *  year otherwise.
 */
const uint16_t   _TOD_Days_since_last_leap_year[4] = { 0, 366, 731, 1096 };

#else

/** @brief Convert Month to Julian Days
 *
 *  This is a prototype for a table which assists in converting the 
 *  current day into the Julian days since the first of the year.
 */
extern const uint16_t   _TOD_Days_to_date[2][13]; /* Julian days */

/** @brief Days since Beginning of Last Leap Year
 *
 *  This is a prototype for a table which assists in calculating the
 *  number of days since the beginning of the last leap year.
 */
extern const uint16_t   _TOD_Days_since_last_leap_year[4];

/** @brief Days Per Month Table
 *
 *  This is a prototype for a table which holds the number of days
 *  per month for a leap year and non-leap year.
 */
extern const uint32_t   _TOD_Days_per_month[2][13];

#endif

/** @brief _TOD_Handler_initialization
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _TOD_Handler_initialization(
  uint32_t   microseconds_per_tick
);

/** @brief _TOD_Set
 *
 *  This routine sets the current time of day to THE_TOD and
 *  the equivalent SECONDS_SINCE_EPOCH.
 */
void _TOD_Set(
  TOD_Control       *the_tod,
  Watchdog_Interval  seconds_since_epoch
);

/** @brief _TOD_Validate
 *
 *  This function returns TRUE if THE_TOD contains
 *  a valid time of day, and FALSE otherwise.
 */
boolean _TOD_Validate(
  TOD_Control *the_tod
);

/** @brief _TOD_To_seconds
 *
 *  This function returns the number seconds between the epoch and THE_TOD.
 */
Watchdog_Interval _TOD_To_seconds(
  TOD_Control *the_tod
);

/** @brief _TOD_Tickle
 *
 *  This routine is scheduled as a watchdog function and is invoked at
 *  each second boundary.  It updates the current time of day to indicate
 *  that a second has passed and processes the seconds watchdog chain.
 */
void _TOD_Tickle(
  Objects_Id  id,
  void       *ignored
);

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

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tod.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
