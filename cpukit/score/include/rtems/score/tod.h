/*  tod.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Time of Day Handler.
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

#ifndef __TIME_OF_DAY_h
#define __TIME_OF_DAY_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/watchdog.h>

/*
 *  The following constants are related to the time of day.
 */

#define TOD_SECONDS_PER_MINUTE (unsigned32)60
#define TOD_MINUTES_PER_HOUR   (unsigned32)60
#define TOD_MONTHS_PER_YEAR    (unsigned32)12
#define TOD_DAYS_PER_YEAR      (unsigned32)365
#define TOD_HOURS_PER_DAY      (unsigned32)24
#define TOD_SECONDS_PER_DAY    (unsigned32) (TOD_SECONDS_PER_MINUTE * \
                                TOD_MINUTES_PER_HOUR   * \
                                TOD_HOURS_PER_DAY)

#define TOD_SECONDS_PER_NON_LEAP_YEAR (365 * TOD_SECONDS_PER_DAY)

#define TOD_MILLISECONDS_PER_SECOND     (unsigned32)1000
#define TOD_MICROSECONDS_PER_SECOND     (unsigned32)1000000
#define TOD_NANOSECONDS_PER_SECOND      (unsigned32)1000000000
#define TOD_NANOSECONDS_PER_MICROSECOND (unsigned32)1000

/*
 *  The following constant define the earliest year to which an
 *  time of day can be initialized.  This is considered the
 *  epoch.
 */

#define TOD_BASE_YEAR 1988

/*
 *  The following record defines the time of control block.  This
 *  control block is used to maintain the current time of day.
 */

typedef struct {                   /* RTEID style time/date */
  unsigned32 year;                 /* year, A.D. */
  unsigned32 month;                /* month, 1 -> 12 */
  unsigned32 day;                  /* day, 1 -> 31 */
  unsigned32 hour;                 /* hour, 0 -> 23 */
  unsigned32 minute;               /* minute, 0 -> 59 */
  unsigned32 second;               /* second, 0 -> 59 */
  unsigned32 ticks;                /* elapsed ticks between secs */
}   TOD_Control;

/*
 *  The following is TRUE if the application has set the current
 *  time of day, and FALSE otherwise.
 */

SCORE_EXTERN boolean _TOD_Is_set;

/*
 *  The following contains the current time of day.
 */

SCORE_EXTERN TOD_Control _TOD_Current;

/*
 *  The following contains the number of seconds from 00:00:00
 *  January 1, TOD_BASE_YEAR until the current time of day.
 */

SCORE_EXTERN Watchdog_Interval _TOD_Seconds_since_epoch;

/*
 *  The following contains the number of microseconds per tick.
 */

SCORE_EXTERN unsigned32 _TOD_Microseconds_per_tick;

/*
 *  The following contains the number of clock ticks per second.
 *
 *  NOTE:
 *
 *  If one second is NOT evenly divisible by the number of microseconds
 *  per clock tick, this value will contain only the integer portion
 *  of the division.  This means that the interval between clock ticks
 *  can be a source of error in the current time of day.
 */

SCORE_EXTERN unsigned32 _TOD_Ticks_per_second;

/*
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

const unsigned32 _TOD_Days_per_month[ 2 ][ 13 ] = {
  { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
  { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

/*
 *  The following array contains the number of days in all months
 *  up to the month indicated by the index of the second dimension.
 *  The first dimension should be 1 for leap years, and 0 otherwise.
 */

const unsigned16 _TOD_Days_to_date[2][13] = {
  { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 },
  { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }
};

/*
 *  The following array contains the number of days in the years
 *  since the last leap year.  The index should be 0 for leap
 *  years, and the number of years since the beginning of a leap
 *  year otherwise.
 */

const unsigned16 _TOD_Days_since_last_leap_year[4] = { 0, 366, 731, 1096 };

#else

extern const unsigned16 _TOD_Days_to_date[2][13]; /* Julian days */
extern const unsigned16 _TOD_Days_since_last_leap_year[4];
extern const unsigned32 _TOD_Days_per_month[2][13];

#endif

/*
 *  _TOD_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

void _TOD_Handler_initialization(
  unsigned32 microseconds_per_tick
);

/*
 *  _TOD_Set
 *
 *  DESCRIPTION:
 *
 *  This routine sets the current time of day to THE_TOD and
 *  the equivalent SECONDS_SINCE_EPOCH.
 */

void _TOD_Set(
  TOD_Control       *the_tod,
  Watchdog_Interval  seconds_since_epoch
);

/*
 *  _TOD_Validate
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if THE_TOD contains
 *  a valid time of day, and FALSE otherwise.
 */

boolean _TOD_Validate(
  TOD_Control *the_tod
);

/*
 *  _TOD_To_seconds
 *
 *  DESCRIPTION:
 *
 *  This function returns the number seconds between the epoch and THE_TOD.
 */

Watchdog_Interval _TOD_To_seconds(
  TOD_Control *the_tod
);

/*
 *  _TOD_Tickle
 *
 *  DESCRIPTION:
 *
 *  This routine is scheduled as a watchdog function and is invoked at
 *  each second boundary.  It updates the current time of day to indicate
 *  that a second has passed and processes the seconds watchdog chain.
 */

void _TOD_Tickle(
  Objects_Id  id,
  void       *ignored
);

/*
 *  TOD_MILLISECONDS_TO_MICROSECONDS
 *
 *  DESCRIPTION:
 *
 *  This routine converts an interval expressed in milliseconds to microseconds.
 *
 *  NOTE:
 *
 *  This must be a macro so it can be used in "static" tables.
 */

#define TOD_MILLISECONDS_TO_MICROSECONDS(_ms) ((_ms) * 1000)

/*
 *  TOD_MICROSECONDS_TO_TICKS
 *
 *  DESCRIPTION:
 *
 *  This routine converts an interval expressed in microseconds to ticks.
 *
 *  NOTE:
 *
 *  This must be a macro so it can be used in "static" tables.
 */

#define TOD_MICROSECONDS_TO_TICKS(_us) \
    ((_us) / _TOD_Microseconds_per_tick)

/*
 *  TOD_MILLISECONDS_TO_TICKS
 *
 *  DESCRIPTION:
 *
 *  This routine converts an interval expressed in milliseconds to ticks.
 *
 *  NOTE:
 *
 *  This must be a macro so it can be used in "static" tables.
 */

#define TOD_MILLISECONDS_TO_TICKS(_ms) \
    (TOD_MILLISECONDS_TO_MICROSECONDS(_ms) / _TOD_Microseconds_per_tick)

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/tod.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
