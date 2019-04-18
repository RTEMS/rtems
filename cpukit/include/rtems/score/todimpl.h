/**
 * @file
 *
 * @ingroup RTEMSScoreTOD
 *
 * @brief Time of Day Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TODIMPL_H
#define _RTEMS_SCORE_TODIMPL_H

#include <rtems/score/timestamp.h>
#include <rtems/score/timecounterimpl.h>
#include <rtems/score/watchdog.h>

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
 * @brief Time of Day Handler
 *
 * The following constants are related to the time of day and are
 * independent of RTEMS.
 *
 * @{
 */

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
 *  This constant represents the number of millisecond in a second.
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
 *  This constant represents the number of nanoseconds in a mircosecond.
 */
#define TOD_NANOSECONDS_PER_MICROSECOND (uint32_t)1000

/**@}*/

/**
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core. The timespec format time
 *  is kept in POSIX compliant form.
 */
#define TOD_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))

/**
 *  @brief Earliest year to which an time of day can be initialized.
 *
 *  The following constant define the earliest year to which an
 *  time of day can be initialized.  This is considered the
 *  epoch.
 */
#define TOD_BASE_YEAR 1988

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

extern TOD_Control _TOD;

/**
 * @brief Locks the time of day mutex.
 */
void _TOD_Lock( void );

/**
 * @brief Unlocks the time of day mutex.
 */
void _TOD_Unlock( void );

/**
 * @brief Checks if api mutex is owner of the time of day mutex.
 *
 * @retval true It is owner of the time of day mutex.
 * @retval false It is not owner of the time of day mutex.
 */
#if defined(RTEMS_DEBUG)
bool _TOD_Is_owner( void );
#endif

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
 * @brief Sets the time of day.
 *
 * The caller must be the owner of the TOD lock.
 *
 * @param tod The new time of day in timespec format representing
 *   the time since UNIX Epoch.
 * @param lock_context The ISR lock context used for the corresponding
 *   _TOD_Acquire().  The caller must be the owner of the TOD lock.  This
 *   function will release the TOD lock.
 */
void _TOD_Set(
  const struct timespec *tod,
  ISR_lock_Context      *lock_context
);

/**
 * @brief Gets the current time in the timespec format.
 *
 * @param[out] time The value gathered by the request.
 */
static inline void _TOD_Get(
  struct timespec *tod
)
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
static inline void _TOD_Get_uptime(
  Timestamp_Control *time
)
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
static inline void _TOD_Get_zero_based_uptime(
  Timestamp_Control *time
)
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
static inline uint32_t _TOD_Seconds_since_epoch( void )
{
  return (uint32_t) _Timecounter_Time_second;
}

/**
 * @brief Gets number of ticks in a second.
 *
 * This method returns the number of ticks in a second.
 *
 * @note If the clock tick value does not multiply evenly into a second
 *       then this number of ticks will be slightly shorter than a second.
 *
 * @return The number of ticks in a second.
 */
uint32_t TOD_TICKS_PER_SECOND_method(void);

/**
 *  @brief Gets number of ticks in a second.
 *
 *  This method exists to hide the fact that TOD_TICKS_PER_SECOND can not
 *  be implemented as a macro in a .h file due to visibility issues.
 *  The Configuration Table is not available to SuperCore .h files but
 *  is available to their .c files.
 */
#define TOD_TICKS_PER_SECOND TOD_TICKS_PER_SECOND_method()

/**
 * @brief This routine returns a timeval based upon the internal timespec
 *      format TOD.
 *
 * @param[out] time The timeval to be filled in by the method.
 */
RTEMS_INLINE_ROUTINE void _TOD_Get_timeval(
  struct timeval *time
)
{
  _Timecounter_Microtime( time );
}

/**
 * @brief Adjusts the Time of Time.
 *
 * This method is used to adjust the current time of day by the
 * specified amount.
 *
 * @param delta is the amount to adjust.
 */
void _TOD_Adjust(
  const struct timespec *delta
);

/**
 * @brief Check if the TOD is Set
 *
 * @retval true The time is set.
 * @retval false The time is not set.
 */
RTEMS_INLINE_ROUTINE bool _TOD_Is_set( void )
{
  return _TOD.is_set;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
