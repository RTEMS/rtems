/**
 * @file
 *
 * @ingroup ScoreTOD
 *
 * @brief Time of Day Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TODIMPL_H
#define _RTEMS_SCORE_TODIMPL_H

#include <rtems/score/tod.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/timestamp.h>

#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreTOD Time of Day Handler
 *
 *  @ingroup Score
 *
 *  The following constants are related to the time of day and are
 *  independent of RTEMS.
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
 *  @defgroup ScoreTOD Time Of Day (TOD) Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality used to manage time of day.
 */
/**@{*/

/**
 *  @brief TOD control.
 */
typedef struct {
  /**
   * @brief Current time of day value.
   *
   * This field is protected by the lock.
   */
  Timestamp_Control now;

  /**
   * @brief System uptime.
   *
   * This field is protected by the lock.
   */
  Timestamp_Control uptime;

  /**
   * @brief Lock to protect the now and uptime fields.
   */
  ISR_lock_Control lock;

  /**
   * @brief Time of day seconds trigger.
   *
   * This value specifies the nanoseconds since the last time of day second.
   * It is updated and evaluated in _TOD_Tickle_ticks().  It is set in
   * _TOD_Set_with_timestamp().
   */
  uint32_t seconds_trigger;

  /**
   * @brief The current nanoseconds since last tick handler.
   *
   * This field must not be NULL after initialization.
   */
  TOD_Nanoseconds_since_last_tick_routine nanoseconds_since_last_tick;

  /**
   *  @brief Indicates if the time of day is set.
   *
   *  This is true if the application has set the current
   *  time of day, and false otherwise.
   */
  bool is_set;
} TOD_Control;

SCORE_EXTERN TOD_Control _TOD;

#define _TOD_Acquire( _tod, _isr_cookie ) \
  _ISR_lock_ISR_disable_and_acquire( &( _tod )->lock, _isr_cookie )

#define _TOD_Release( _tod, _isr_cookie ) \
  _ISR_lock_Release_and_ISR_enable( &( _tod )->lock, _isr_cookie )

/**
 *  @brief Initializes the time of day handler.
 *
 *  Performs the initialization necessary for the Time Of Day handler.
 */
void _TOD_Handler_initialization(void);

/**
 *  @brief Sets the time of day from timestamp.
 *
 *  The @a tod_as_timestamp timestamp represents the time since UNIX epoch.
 *  The watchdog seconds chain will be adjusted.
 *
 *  @param[in] tod_as_timestamp is the constant of the time of day as a timestamp
 */
void _TOD_Set_with_timestamp(
  const Timestamp_Control *tod_as_timestamp
);

static inline void _TOD_Set(
  const struct timespec *tod_as_timespec
)
{
  Timestamp_Control tod_as_timestamp;

  _Timestamp_Set(
    &tod_as_timestamp,
    tod_as_timespec->tv_sec,
    tod_as_timespec->tv_nsec
  );
  _TOD_Set_with_timestamp( &tod_as_timestamp );
}

/**
 *  @brief Returns a snapshot of a clock.
 *
 *  This function invokes the nanoseconds extension.
 *
 *  @param[out] snapshot points to an area that will contain the current
 *              TOD plus the BSP nanoseconds since last tick adjustment
 *  @param[in] clock contains the current TOD
 *
 *  @retval @a snapshot
 */
Timestamp_Control *_TOD_Get_with_nanoseconds(
  Timestamp_Control *snapshot,
  const Timestamp_Control *clock
);

static inline void _TOD_Get(
  struct timespec *tod_as_timespec
)
{
  Timestamp_Control  tod_as_timestamp;
  Timestamp_Control *tod_as_timestamp_ptr;

  tod_as_timestamp_ptr =
    _TOD_Get_with_nanoseconds( &tod_as_timestamp, &_TOD.now );
  _Timestamp_To_timespec( tod_as_timestamp_ptr, tod_as_timespec );
}

/**
 *  @brief Gets the system uptime with potential accuracy to the nanosecond.
 *
 *  This routine returns the system uptime with potential accuracy
 *  to the nanosecond.
 *
 *  @param[in] time is a pointer to the uptime to be returned
 */
static inline void _TOD_Get_uptime(
  Timestamp_Control *time
)
{
  _TOD_Get_with_nanoseconds( time, &_TOD.uptime );
}

/**
 *  @brief Gets the system uptime with potential accuracy to the nanosecond.
 *
 *  This routine returns the system uptime with potential accuracy
 *  to the nanosecond.
 *
 *  @param[in] time is a pointer to the uptime to be returned
 */
void _TOD_Get_uptime_as_timespec(
  struct timespec *time
);

/**
 *  @brief Number of seconds Since RTEMS epoch.
 *
 *  The following contains the number of seconds from 00:00:00
 *  January 1, TOD_BASE_YEAR until the current time of day.
 */
uint32_t _TOD_Seconds_since_epoch( void );

/**
 *  @brief Increments time of day at each clock tick.
 *
 *  This routine increments the ticks field of the current time of
 *  day at each clock tick.
 */
void _TOD_Tickle_ticks( void );

/**
 *  @brief Gets number of ticks in a second.
 *
 *  This method returns the number of ticks in a second.
 *
 *  @note If the clock tick value does not multiply evenly into a second
 *        then this number of ticks will be slightly shorter than a second.
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
 * This routine returns a timeval based upon the internal timespec format TOD.
 */

RTEMS_INLINE_ROUTINE void _TOD_Get_timeval(
  struct timeval *time
)
{
  Timestamp_Control  snapshot_as_timestamp;
  Timestamp_Control *snapshot_as_timestamp_ptr;

  snapshot_as_timestamp_ptr =
    _TOD_Get_with_nanoseconds( &snapshot_as_timestamp, &_TOD.now );
  _Timestamp_To_timeval( snapshot_as_timestamp_ptr, time );
}

RTEMS_INLINE_ROUTINE void _TOD_Set_nanoseconds_since_last_tick_handler(
  TOD_Nanoseconds_since_last_tick_routine routine
)
{
  _TOD.nanoseconds_since_last_tick = routine;
}

RTEMS_INLINE_ROUTINE bool _TOD_Is_set( void )
{
  return _TOD.is_set;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
