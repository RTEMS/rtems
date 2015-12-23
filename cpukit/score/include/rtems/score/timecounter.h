/**
 * @file
 *
 * @ingroup ScoreTimecounter
 *
 * @brief Timecounter API
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_TIMECOUNTER_H
#define _RTEMS_SCORE_TIMECOUNTER_H

#include <sys/time.h>
#include <sys/timetc.h>

#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreTimecounter Timecounter Handler
 *
 * @ingroup Score
 *
 * @{
 */

/**
 * @brief Returns the wall clock time in the bintime format.
 *
 * @param[out] bt Returns the wall clock time.
 */
void _Timecounter_Bintime( struct bintime *bt );

/**
 * @brief Returns the wall clock time in the timespec format.
 *
 * @param[out] ts Returns the wall clock time.
 */
void _Timecounter_Nanotime( struct timespec *ts );

/**
 * @brief Returns the wall clock time in the timeval format.
 *
 * @param[out] tv Returns the wall clock time.
 */
void _Timecounter_Microtime( struct timeval *tv );

/**
 * @brief Returns the uptime in the bintime format.
 *
 * @param[out] bt Returns the uptime.
 */
void _Timecounter_Binuptime( struct bintime *bt );

/**
 * @brief Returns the uptime in the timespec format.
 *
 * @param[out] ts Returns the uptime.
 */
void _Timecounter_Nanouptime( struct timespec *ts );

/**
 * @brief Returns the uptime in the timeval format.
 *
 * @param[out] tv Returns the uptime.
 */
void _Timecounter_Microuptime( struct timeval *tv );

/**
 * @brief Returns the wall clock time in the bintime format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Bintime() variant.
 *
 * @param[out] ts Returns the wall clock time.
 */
void _Timecounter_Getbintime( struct bintime *bt );

/**
 * @brief Returns the wall clock time in the timespec format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Nanotime() variant.
 *
 * @param[out] ts Returns the wall clock time.
 *
 * @see _Timecounter_Getbintime().
 */
void _Timecounter_Getnanotime( struct timespec *ts );

/**
 * @brief Returns the wall clock time in the timeval format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Microtime() variant.
 *
 * @param[out] tv Returns the wall clock time.
 *
 * @see _Timecounter_Getbintime().
 */
void _Timecounter_Getmicrotime( struct timeval *tv );

/**
 * @brief Returns the uptime in the bintime format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Binuptime() variant.
 *
 * @param[out] ts Returns the uptime.
 */
void _Timecounter_Getbinuptime( struct bintime *bt );

/**
 * @brief Returns the uptime in the timespec format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Nanouptime() variant.
 *
 * @param[out] ts Returns the uptime.
 */
void _Timecounter_Getnanouptime( struct timespec *ts );

/**
 * @brief Returns the uptime in the timeval format.
 *
 * This function obtains the time with a lower overhead and lower accuracy
 * compared to the _Timecounter_Microuptime() variant.
 *
 * @param[out] tv Returns the uptime.
 */
void _Timecounter_Getmicrouptime( struct timeval *tv );

/**
 * @brief Installs the timecounter.
 *
 * The timecounter structure must contain valid values in the fields
 * tc_get_timecount, tc_counter_mask, tc_frequency and tc_quality.  All other
 * fields must be zero initialized.
 *
 * @param[in] tc The timecounter.
 */
void _Timecounter_Install( struct timecounter *tc );

/**
 * @brief Performs a timecounter tick.
 */
void _Timecounter_Tick( void );

/**
 * @brief Lock to protect the timecounter mechanic.
 */
ISR_LOCK_DECLARE( extern, _Timecounter_Lock )

/**
 * @brief Acquires the timecounter lock.
 *
 * @param[in] lock_context The lock context.
 *
 * See _Timecounter_Tick_simple().
 */
#define _Timecounter_Acquire( lock_context ) \
  _ISR_lock_ISR_disable_and_acquire( &_Timecounter_Lock, lock_context )

/**
 * @brief Performs a simple timecounter tick.
 *
 * This is a special purpose tick function for simple timecounter to support
 * legacy clock drivers.
 *
 * @param[in] delta The time in timecounter ticks elapsed since the last call
 * to _Timecounter_Tick_simple().
 * @param[in] offset The current value of the timecounter.
 * @param[in] lock_context The lock context of the corresponding
 * _Timecounter_Acquire().
 */
void _Timecounter_Tick_simple(
  uint32_t delta,
  uint32_t offset,
  ISR_lock_Context *lock_context
);

/**
 * @brief The wall clock time in seconds.
 */
extern volatile time_t _Timecounter_Time_second;

/**
 * @brief The uptime in seconds.
 *
 * For compatibility with the FreeBSD network stack the initial value is one
 * second.
 */
extern volatile time_t _Timecounter_Time_uptime;

/**
 * @brief The current timecounter.
 */
extern struct timecounter *_Timecounter;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_TIMECOUNTER_H */
