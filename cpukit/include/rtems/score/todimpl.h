/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTOD
 * @ingroup RTEMSScoreTODHooks
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreTOD and the @ref RTEMSScoreTODHooks.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_TODIMPL_H
#define _RTEMS_SCORE_TODIMPL_H

#include <rtems/score/status.h>
#include <rtems/score/timestamp.h>
#include <rtems/score/timecounterimpl.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/watchdogticks.h>

#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreTOD Time of Day Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Time of Day Handler implementation.
 *
 * The following constants are related to the time of day and are
 * independent of RTEMS.
 *
 * @{
 */

/**
 *  This constant represents the number of seconds in a minute.
 */
#define TOD_SECONDS_PER_MINUTE (uint32_t) 60

/**
 *  This constant represents the number of minutes per hour.
 */
#define TOD_MINUTES_PER_HOUR (uint32_t) 60

/**
 *  This constant represents the number of months in a year.
 */
#define TOD_MONTHS_PER_YEAR (uint32_t) 12

/**
 *  This constant represents the number of days in a non-leap year.
 */
#define TOD_DAYS_PER_YEAR (uint32_t) 365

/**
 *  This constant represents the number of hours per day.
 */
#define TOD_HOURS_PER_DAY (uint32_t) 24

/**
 *  This constant represents the number of seconds in a day which does
 *  not include a leap second.
 */
#define TOD_SECONDS_PER_DAY                                    \
  (uint32_t) ( TOD_SECONDS_PER_MINUTE * TOD_MINUTES_PER_HOUR * \
               TOD_HOURS_PER_DAY )

/**
 *  This constant represents the number of seconds in a non-leap year.
 */
#define TOD_SECONDS_PER_NON_LEAP_YEAR ( 365 * TOD_SECONDS_PER_DAY )

/**
 *  This constant represents the number of millisecond in a second.
 */
#define TOD_MILLISECONDS_PER_SECOND 1000

/**
 *  This constant represents the number of microseconds in a second.
 */
#define TOD_MICROSECONDS_PER_SECOND 1000000L

/**
 *  This constant represents the number of nanoseconds in a second.
 */
#define TOD_NANOSECONDS_PER_SECOND 1000000000L

/**
 *  This constant represents the number of nanoseconds in a mircosecond.
 */
#define TOD_NANOSECONDS_PER_MICROSECOND 1000

/**@}*/

/**
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core. The timespec format time
 *  is kept in POSIX compliant form.
 */
#define TOD_SECONDS_1970_THROUGH_1988                         \
  ( ( ( 1987 - 1970 + 1 ) * TOD_SECONDS_PER_NON_LEAP_YEAR ) + \
    ( 4 * TOD_SECONDS_PER_DAY ) )

/**
 * @brief Seconds from 1970-01-01T00:00:00Z to 2401-01-01T00:00:00Z.
 *
 * A time of day below this value lies in the range which _TOD_Set() accepts.
 * The year 2400 was chosen to guarantee a defined CLOCK_REALTIME within the
 * range of a system uptime of about 113 years.
 */
#define TOD_SECONDS_1970_THROUGH_2400 13601088000

/**
 *  @brief Earliest year to which an time of day can be initialized.
 *
 *  The following constant define the earliest year to which an
 *  time of day can be initialized.  This is considered the
 *  epoch.
 */
#define TOD_BASE_YEAR 1988

/**
 * @brief Latest year to which a time of day can be initialized.
 *
 * The following constant defines the latest year to which an
 * RTEMS time of day can be set using rtems_clock_set().
 *
 * The year 2400 was chosen to guarantee that the 34-bit seconds counter
 * used by realtime watchdogs does not overflow during a system uptime
 * of more than 113 years.
 */
#define TOD_LATEST_YEAR 2400

/**
 * @addtogroup RTEMSScoreTOD
 *
 * This handler encapsulates functionality used to manage time of day.
 *
 * @{
 */

/**
 *  @brief TOD control.
 */
typedef struct {
  /**
   *  @brief Indicates if the time of day is set.
   *
   *  This is true if the application has set the current
   *  time of day, and false otherwise.
   */
  bool is_set;
} TOD_Control;

/**
 * @brief TOD Management information
 */
extern TOD_Control _TOD;

/**
 * @brief This array contains the number of days in all months up to the month
 *   indicated by the index of the second dimension.
 *
 * The first dimension should be 0 for leap years, and 1 otherwise.
 */
extern const uint16_t _TOD_Days_to_date[ 2 ][ 13 ];

/**
 * @brief Acquires the lock context for the timecounter.
 *
 * @param lock_context The lock to acquire.
 */
static inline void _TOD_Acquire( ISR_lock_Context *lock_context )
{
  _Timecounter_Acquire( lock_context );
}

/**
 * @brief Releases the lock context for the timecounter.
 *
 * @param lock_context The lock to release.
 */
static inline void _TOD_Release( ISR_lock_Context *lock_context )
{
  _Timecounter_Release( lock_context );
}

/**
 * @brief Checks the time point is a valid new time of day for _TOD_Set().
 *
 * @param tod the time of day to check.
 *
 * @retval STATUS_SUCCESSFUL The time of day is valid.
 *
 * @retval STATUS_INVALID_NUMBER The time of day is invalid.
 */
Status_Control _TOD_Is_valid_new_time_of_day( const struct timespec *tod );

/**
 * @brief Sets the time of day.
 *
 * The caller must be the owner of the object allocator mutex.
 *
 * @param tod The new time of day in timespec format representing
 *   the time since UNIX Epoch.  The new time of day shall be valid according
 *   to _TOD_Is_valid_new_time_of_day().
 * @param lock_context The ISR lock context used for the corresponding
 *   _TOD_Acquire().  This function will release the lock of the timecounter.
 *
 * @retval STATUS_SUCCESSFUL Successful operation.
 * @retval other Some error occurred.
 */
Status_Control _TOD_Set(
  const struct timespec *tod,
  ISR_lock_Context      *lock_context
);

/**
 * @brief Gets the current time in the timespec format.
 *
 * @param[out] time The value gathered by the request.
 */
static inline void _TOD_Get( struct timespec *tod )
{
  _Timecounter_Nanotime( tod );
}

/**
 * @brief Gets the system uptime with potential accuracy to the nanosecond.
 *
 * This routine returns the system uptime with potential accuracy
 * to the nanosecond.
 *
 * The initial uptime value is undefined.
 *
 * @param[out] time Is a pointer to the uptime after the method call.
 */
static inline void _TOD_Get_uptime( Timestamp_Control *time )
{
  *time = _Timecounter_Sbinuptime();
}

/**
 * @brief Gets the system uptime with potential accuracy to the nanosecond.
 *
 * The initial uptime value is zero.
 *
 * @param[out] time Is a pointer to the uptime after the method call.
 */
static inline void _TOD_Get_zero_based_uptime( Timestamp_Control *time )
{
  *time = _Timecounter_Sbinuptime() - SBT_1S;
}

/**
 * @brief Gets the system uptime with potential accuracy to the nanosecond.
 *
 * The initial uptime value is zero.
 *
 * @param[out] time Is a pointer to the uptime after the method call.
 */
static inline void _TOD_Get_zero_based_uptime_as_timespec(
  struct timespec *time
)
{
  _Timecounter_Nanouptime( time );
  --time->tv_sec;
}

/**
 * @brief Returns Number of seconds Since RTEMS epoch.
 *
 * The following contains the number of seconds from 00:00:00
 * January 1, TOD_BASE_YEAR until the current time of day.
 *
 * @return The number of seconds since RTEMS epoch.
 */
static inline time_t _TOD_Seconds_since_epoch( void )
{
  return _Timecounter_Time_second;
}

/**
 *  @brief Gets number of ticks in a second.
 */
#define TOD_TICKS_PER_SECOND _Watchdog_Ticks_per_second

/**
 * @brief This routine returns a timeval based upon the internal timespec
 *      format TOD.
 *
 * @param[out] time The timeval to be filled in by the method.
 */
static inline void _TOD_Get_timeval( struct timeval *time )
{
  _Timecounter_Microtime( time );
}

/**
 * @brief Check if the TOD is Set
 *
 * @retval true The time is set.
 * @retval false The time is not set.
 */
static inline bool _TOD_Is_set( void )
{
  return _TOD.is_set;
}

/** @} */

/**
 * @defgroup RTEMSScoreTODHooks Time of Day Handler Action Hooks
 *
 * @ingroup RTEMSScoreTOD
 *
 * @brief This group contains the implementation to support Time of Day Handler
 *   action hooks.
 *
 * The following support registering a hook which is invoked
 * when the TOD is set. These can be used by a paravirtualized
 * BSP to mirror time changes to the hosting environment or a
 * regular BSP to program a real-time clock when the RTEMS TOD
 * is set.
 *
 * @{
 */

/**
 *  @brief Possible actions where a registered hook could be invoked
 */
typedef enum {
  /**
   *  @brief Constant to indicate the TOD is being set.
   */
  TOD_ACTION_SET_CLOCK
} TOD_Action;

/**
 * @brief Structure to manage each TOD action hook
 */
typedef struct TOD_Hook {
  /** This is the chain node portion of an object. */
  Chain_Node Node;

  /** This is the TOD action hook that is invoked. */
  Status_Control ( *handler )( TOD_Action, const struct timespec * );
} TOD_Hook;

/**
 * @brief Set of registered methods for TOD Actions
 */
extern Chain_Control _TOD_Hooks;

/**
 * @brief Add a TOD Action Hook
 *
 * This method is used to add a hook to the TOD action set.
 *
 * @brief hook is the action hook to register.
 */
void _TOD_Hook_Register( TOD_Hook *hook );

/**
 * @brief Remove a TOD Action Hook
 *
 * This method is used to remove a hook from the TOD action set.
 *
 * @brief hook is the action hook to unregister.
 */
void _TOD_Hook_Unregister( TOD_Hook *hook );

/**
 * @brief Run the TOD Action Hooks
 *
 * This method is used to invoke the set of TOD action hooks.
 *
 * @brief action The action which triggered this run.
 * @brief tod The current time of day.
 *
 * @retval STATUS_SUCCESSFUL Successful operation.
 * @retval other Some error occurred.
 */
Status_Control _TOD_Hook_Run( TOD_Action action, const struct timespec *tod );

/**
 * @brief Converts the broken-down time in UTC to the seconds since the Epoch.
 *
 * The musl import in contrib/cpukit/musl provides this function.  The
 * function uses no time zone and sets no errno.
 *
 * @param tm is the broken-down time.  The function reads the members tm_sec,
 *   tm_min, tm_hour, tm_mday, tm_mon, and tm_year.
 *
 * @return Returns the seconds since the Epoch.
 */
long long __tm_to_secs( const struct tm *tm );

/**
 * @brief Converts the seconds since the Epoch to the broken-down time in UTC.
 *
 * The musl import in contrib/cpukit/musl provides this function.  The
 * function uses no time zone and sets no errno.  It sets no member of the
 * broken-down time other than tm_sec, tm_min, tm_hour, tm_mday, tm_mon,
 * tm_year, tm_wday, and tm_yday.
 *
 * @param t is the seconds since the Epoch.
 *
 * @param[out] tm is the broken-down time.
 *
 * @retval 0 The function converted the seconds.
 *
 * @retval -1 The year of the result is not in the range of int.
 */
int __secs_to_tm( long long t, struct tm *tm );

/**
 * @brief Converts the broken-down time in UTC to the seconds since the Epoch.
 *
 * @param tm is the broken-down time.  The function reads the members tm_sec,
 *   tm_min, tm_hour, tm_mday, tm_mon, and tm_year.
 *
 * @return Returns the seconds since the Epoch.
 */
static inline time_t _TOD_Tm_to_seconds( const struct tm *tm )
{
  return (time_t) __tm_to_secs( tm );
}

/**
 * @brief Converts the seconds since the Epoch to the broken-down time in UTC.
 *
 * The function sets no member of the broken-down time other than tm_sec,
 * tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, and tm_yday.
 *
 * @param seconds is the seconds since the Epoch.
 *
 * @param[out] tm is the broken-down time.
 *
 * @retval true The function converted the seconds.
 *
 * @retval false The year of the result is not in the range of int.
 */
static inline bool _TOD_Seconds_to_tm( time_t seconds, struct tm *tm )
{
  return __secs_to_tm( seconds, tm ) == 0;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
