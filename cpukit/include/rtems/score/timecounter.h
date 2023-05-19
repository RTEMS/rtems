/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreTimecounter
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreTimecounter which are used by the implementation and the API.
 */

/*
 * Copyright (C) 2015, 2021 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_TIMECOUNTER_H
#define _RTEMS_SCORE_TIMECOUNTER_H

#include <sys/time.h>
#include <sys/timetc.h>
#include <machine/_timecounter.h>

#include <rtems/score/isrlock.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreTimecounter Timecounter Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Timecounter Handler implementation.
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
 * @brief Returns the uptime in the sbintime_t format.
 *
 * @return Returns the uptime.
 */
int64_t _Timecounter_Sbinuptime( void );

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
 * @brief Returns the boot time in the timeval format.
 *
 * @param[out] tv Returns the boot time.
 */
void _Timecounter_Getboottime( struct timeval *tv );

/**
 * @brief Returns the boot time in the bintime format.
 *
 * @param[out] tv Returns the boot time.
 */
void _Timecounter_Getboottimebin( struct bintime *bt );

/**
 * @brief Installs the timecounter.
 *
 * The timecounter structure must contain valid values in the fields
 * tc_get_timecount, tc_counter_mask, tc_frequency and tc_quality.  All other
 * fields must be zero initialized.
 *
 * @param tc The timecounter.
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
 * @param lock_context The lock context.
 *
 * See _Timecounter_Tick_simple().
 */
#define _Timecounter_Acquire( lock_context ) \
  _ISR_lock_ISR_disable_and_acquire( &_Timecounter_Lock, lock_context )

/**
 * @brief Releases the timecounter lock.
 *
 * @param lock_context The lock context.
 *
 * See _Timecounter_Tick_simple().
 */
#define _Timecounter_Release(lock_context) \
   _ISR_lock_Release_and_ISR_enable(&_Timecounter_Lock, lock_context)

/**
 * @brief Performs a simple timecounter tick.
 *
 * This is a special purpose tick function for simple timecounter to support
 * legacy clock drivers.
 *
 * @param delta The time in timecounter ticks elapsed since the last call
 * to _Timecounter_Tick_simple().
 * @param offset The current value of the timecounter.
 * @param] lock_context The lock context of the corresponding
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
extern volatile int32_t _Timecounter_Time_uptime;

/**
 * @brief The current timecounter.
 */
extern struct timecounter *_Timecounter;

/**
 * @brief Handler doing the NTP update second processing shall have this type.
 *
 * @param[in, out] adjustment is the NTP time adjustment.
 *
 * @param[in, out] newsec is the number of seconds since Unix epoch.
 */
typedef	void ( *Timecounter_NTP_update_second )(
  int64_t *adjustment,
  time_t  *newsec
);

/**
 * @brief Sets the NTP update second handler.
 *
 * @param handler is the new NTP update second handler used to carry out the
 *   NTP update second processing.
 */
void _Timecounter_Set_NTP_update_second(
  Timecounter_NTP_update_second handler
);

/**
 * @brief Updates the time adjustment and seconds according to the NTP state.
 *
 * @param[in, out] adjustment is the NTP time adjustment.
 *
 * @param[in, out] newsec is the number of seconds since Unix epoch.
 */
void _Timecounter_NTP_update_second(int64_t *adjustment, time_t *newsec);

/**
 * @brief Gets the frequency in Hz of the current timecounter at some time
 *   point during the call.
 *
 * @return Returns the frequency in Hz.
 */
uint64_t _Timecounter_Get_frequency(void);

/**
 * @brief Updates the timecounter frequency adjustment used by
 * _Timecounter_Set_NTP_update_second().
 *
 * This function is part of the time synchronization using a PPS
 * (Pulse Per Second) signal.
 *
 * When an event (a rising or falling edge of the PPS signal) occurs, the
 * functions pps_capture() and pps_event() are executed. Only if the kernel
 * consumer is configured to be PPS_KC_HARDPPS, the timecounter is disciplined.
 *
 * @param[in] tsp is the time at PPS event
 *
 * @param[i] nsec is the time in nanoseconds from the last PPS event
 */
void _Timecounter_Discipline(struct timespec *tsp, long nsec);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_TIMECOUNTER_H */
