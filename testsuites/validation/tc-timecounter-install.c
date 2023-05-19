/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTimecounterValInstall
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/timecounter.h>
#include <rtems/score/atomic.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTimecounterValInstall spec:/score/timecounter/val/install
 *
 * @ingroup TestsuitesValidationTimecounter0
 *
 * @brief Tests timecounter installation related functions and directives of
 *   the Clock Manager.
 *
 * This test case performs the following actions:
 *
 * - Call the simple timecounter tick service with a zero delta and offset.
 *   This will lead to an overflow to zero of the timehand generation.  It
 *   shall not change the initial clock values.
 *
 * - Call the directives to get the initial value of CLOCK_REALTIME and the
 *   initial boot time.
 *
 *   - Check the initial CLOCK_REALTIME in seconds and nanoseconds format.
 *
 *   - Check that CLOCK_REALTIME is frozen in seconds and nanoseconds format.
 *
 *   - Check the initial CLOCK_REALTIME in coarse resolution in seconds and
 *     nanoseconds format.
 *
 *   - Check that CLOCK_REALTIME is frozen in coarse resolution in seconds and
 *     nanoseconds format.
 *
 *   - Check the initial CLOCK_REALTIME in binary time format.
 *
 *   - Check that CLOCK_REALTIME is frozen in binary time format.
 *
 *   - Check the initial CLOCK_REALTIME in coarse resolution in binary time
 *     format.
 *
 *   - Check that CLOCK_REALTIME is frozen in coarse resolution in binary time
 *     format.
 *
 *   - Check the initial CLOCK_REALTIME in seconds and microseconds format.
 *
 *   - Check that CLOCK_REALTIME is frozen in seconds and microseconds format.
 *
 *   - Check the initial CLOCK_REALTIME in coarse resolution in seconds and
 *     microseconds format.
 *
 *   - Check that CLOCK_REALTIME is frozen in coarse resolution in seconds and
 *     microseconds format.
 *
 *   - Check the initial boot time in seconds and nanoseconds format.
 *
 *   - Check the initial boot time in binary time format.
 *
 *   - Check the initial boot time in seconds and microseconds format.
 *
 * - Call the directives to get the initial value of CLOCK_MONOTONIC and the
 *   initial boot time.
 *
 *   - Check the initial CLOCK_MONOTONIC in seconds and nanoseconds format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in seconds and nanoseconds format.
 *
 *   - Check the initial CLOCK_MONOTONIC in coarse resolution in seconds and
 *     nanoseconds format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in coarse resolution in seconds and
 *     nanoseconds format.
 *
 *   - Check the initial CLOCK_MONOTONIC in binary time format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in binary time format.
 *
 *   - Check the initial CLOCK_MONOTONIC in coarse resolution in binary time
 *     format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in coarse resolution in binary time
 *     format.
 *
 *   - Check the initial CLOCK_MONOTONIC in signed binary time format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in signed binary time format.
 *
 *   - Check the initial CLOCK_MONOTONIC in seconds and microseconds format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in seconds and microseconds format.
 *
 *   - Check the initial CLOCK_MONOTONIC in coarse resolution in seconds and
 *     microseconds format.
 *
 *   - Check that CLOCK_MONOTONIC is frozen in coarse resolution in seconds and
 *     microseconds format.
 *
 * - Install timecounter of different quality levels and frequencies.
 *
 *   - Install a timecounter with a high quality level and normal frequency.
 *     Check that it was installed.
 *
 *   - Install a timecounter with a high quality level and low frequency. Check
 *     that it was not installed.
 *
 *   - Install a timecounter with a high quality level and high frequency.
 *     Check that it was installed.
 *
 *   - Install a timecounter with a low quality level.  Check that it was not
 *     installed.
 *
 * - Call the directives to get the time in the highest resolution available to
 *   the system.
 *
 *   - Check that the timecounter was used by rtems_clock_get_realtime().
 *
 *   - Check that the timecounter was used by
 *     rtems_clock_get_realtime_bintime().
 *
 *   - Check that the timecounter was used by
 *     rtems_clock_get_realtime_timeval().
 *
 *   - Check that the timecounter was used by rtems_clock_get_monotonic().
 *
 *   - Check that the timecounter was used by
 *     rtems_clock_get_monotonic_bintime().
 *
 *   - Check that the timecounter was used by
 *     rtems_clock_get_monotonic_sbintime().
 *
 *   - Check that the timecounter was used by
 *     rtems_clock_get_monotonic_timeval().
 *
 * - Call the directives to get the time in a coarse resolution.
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_realtime_coarse().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_realtime_coarse_bintime().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_realtime_coarse_timeval().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_monotonic_coarse().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_monotonic_coarse_bintime().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_monotonic_coarse_timeval().
 *
 *   - Check that the timecounter was not used by rtems_clock_get_boot_time().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_boot_time_bintime().
 *
 *   - Check that the timecounter was not used by
 *     rtems_clock_get_boot_time_timeval().
 *
 * - Call the directives to get the time in the highest resolution available to
 *   the system.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_realtime() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_realtime_bintime() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_realtime_timeval() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_monotonic() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_monotonic_bintime() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_monotonic_sbintime() returns the correct time.
 *
 *   - Prepare the timecounter to get a large time difference.  Check that
 *     rtems_clock_get_monotonic_timeval() returns the correct time.
 *
 * - Update the oldest timehand after a large time interval.
 *
 * - Call the simple timecounter tick service with non-zero delta and offset
 *   parameter values so that exactly one second passed.
 *
 *   - Check that exactly one second passed due to the simple clock tick
 *     service.
 *
 * - Install a very high quality timecounter with a low frequency to test the
 *   NTP support.
 *
 *   - Let the seconds value of CLOCK_REALTIME not change. Check that the NTP
 *     update second handler is not called.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by one. Check that the
 *     NTP update second handler is called exactly once.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by 200. Check that the
 *     NTP update second handler is called exactly 200 times.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by 201. Check that the
 *     NTP update second handler is called exactly twice.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by one. Check that the
 *     NTP update second handler is incremented the CLOCK_REALTIME by one
 *     second.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by one. Check that the
 *     NTP update second handler is decremented the CLOCK_REALTIME by one
 *     second.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by one. Check that the
 *     NTP update second handler increased the timecounter frequency.
 *
 *   - Let the seconds value of CLOCK_REALTIME change by one. Check that the
 *     NTP update second handler decreased the timecounter frequency.
 *
 * @{
 */

typedef struct {
  struct timecounter base;
  Atomic_Ulong counter;
} Timecounter;

static Timecounter high_quality_low_frequency;

static Timecounter high_quality_normal_frequency;

static Timecounter high_quality_high_frequency;

static Timecounter low_quality;

static Timecounter very_high_quality;

static uint32_t ntp_counter;

static uint32_t GetTimecount( struct timecounter *base )
{
  Timecounter *tc;

  tc = (Timecounter *) base;

  return (uint32_t) _Atomic_Fetch_add_ulong(
    &tc->counter,
    1,
    ATOMIC_ORDER_RELAXED
  );
}

static uint32_t GetCounter( const Timecounter *tc )
{
  return (uint32_t) _Atomic_Load_ulong(
    &tc->counter,
    ATOMIC_ORDER_RELAXED
  );
}

static void SetCounter( Timecounter *tc, uint32_t counter )
{
  _Atomic_Store_ulong(
    &tc->counter,
    counter,
    ATOMIC_ORDER_RELAXED
  );
}

static void NtpUpdateCounter( int64_t *adjustment, time_t *newsec )
{
  (void) newsec;
  T_eq_i64( *adjustment, 0 );
  ++ntp_counter;
}

static void NtpUpdateSecondIncrement( int64_t *adjustment, time_t *newsec )
{
  (void) adjustment;
  ++(*newsec);
}

static void NtpUpdateSecondDecrement( int64_t *adjustment, time_t *newsec )
{
  (void) adjustment;
  --(*newsec);
}

static void NtpUpdateAdjustmentFaster( int64_t *adjustment, time_t *newsec )
{
  *adjustment = ( (int64_t) 5000 ) << 32;
  (void) newsec;
}

static void NtpUpdateAdjustmentSlower( int64_t *adjustment, time_t *newsec )
{
  *adjustment = -( (int64_t) 5000 ) << 32;
  (void) newsec;
}

static void CallTimecounterTick( void )
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  rtems_timecounter_tick();
  _Thread_Dispatch_enable( cpu_self );
}

/**
 * @brief Call the simple timecounter tick service with a zero delta and
 *   offset. This will lead to an overflow to zero of the timehand generation.
 *   It shall not change the initial clock values.
 */
static void ScoreTimecounterValInstall_Action_0( void )
{
  ISR_lock_Context lock_context;

  _Timecounter_Acquire( &lock_context );
  _Timecounter_Tick_simple( 0, 0, &lock_context );
}

/**
 * @brief Call the directives to get the initial value of CLOCK_REALTIME and
 *   the initial boot time.
 */
static void ScoreTimecounterValInstall_Action_1( void )
{
  struct bintime  bt;
  struct timespec ts;
  struct timeval  tv;

  /*
   * Check the initial CLOCK_REALTIME in seconds and nanoseconds format.
   */
  rtems_clock_get_realtime( &ts );
  T_eq_i64( ts.tv_sec, 567993600 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in seconds and nanoseconds format.
   */
  rtems_clock_get_realtime( &ts );
  T_eq_i64( ts.tv_sec, 567993600 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check the initial CLOCK_REALTIME in coarse resolution in seconds and
   * nanoseconds format.
   */
  rtems_clock_get_realtime_coarse( &ts );
  T_eq_i64( ts.tv_sec, 567993600 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in coarse resolution in seconds and
   * nanoseconds format.
   */
  rtems_clock_get_realtime_coarse( &ts );
  T_eq_i64( ts.tv_sec, 567993600 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check the initial CLOCK_REALTIME in binary time format.
   */
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993600 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in binary time format.
   */
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993600 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check the initial CLOCK_REALTIME in coarse resolution in binary time
   * format.
   */
  rtems_clock_get_realtime_coarse_bintime( &bt );
  T_eq_i64( bt.sec, 567993600 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in coarse resolution in binary time
   * format.
   */
  rtems_clock_get_realtime_coarse_bintime( &bt );
  T_eq_i64( bt.sec, 567993600 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check the initial CLOCK_REALTIME in seconds and microseconds format.
   */
  rtems_clock_get_realtime_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993600 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in seconds and microseconds format.
   */
  rtems_clock_get_realtime_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993600 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check the initial CLOCK_REALTIME in coarse resolution in seconds and
   * microseconds format.
   */
  rtems_clock_get_realtime_coarse_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993600 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check that CLOCK_REALTIME is frozen in coarse resolution in seconds and
   * microseconds format.
   */
  rtems_clock_get_realtime_coarse_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993600 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check the initial boot time in seconds and nanoseconds format.
   */
  rtems_clock_get_boot_time( &ts );
  T_eq_i64( ts.tv_sec, 567993599 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check the initial boot time in binary time format.
   */
  rtems_clock_get_boot_time_bintime( &bt );
  T_eq_i64( bt.sec, 567993599 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check the initial boot time in seconds and microseconds format.
   */
  rtems_clock_get_boot_time_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993599 );
  T_eq_long( tv.tv_usec, 0 );
}

/**
 * @brief Call the directives to get the initial value of CLOCK_MONOTONIC and
 *   the initial boot time.
 */
static void ScoreTimecounterValInstall_Action_2( void )
{
  struct bintime  bt;
  sbintime_t      sb;
  struct timespec ts;
  struct timeval  tv;

  /*
   * Check the initial CLOCK_MONOTONIC in seconds and nanoseconds format.
   */
  rtems_clock_get_monotonic( &ts );
  T_eq_i64( ts.tv_sec, 1 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in seconds and nanoseconds format.
   */
  rtems_clock_get_monotonic( &ts );
  T_eq_i64( ts.tv_sec, 1 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check the initial CLOCK_MONOTONIC in coarse resolution in seconds and
   * nanoseconds format.
   */
  rtems_clock_get_monotonic_coarse( &ts );
  T_eq_i64( ts.tv_sec, 1 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in coarse resolution in seconds and
   * nanoseconds format.
   */
  rtems_clock_get_monotonic_coarse( &ts );
  T_eq_i64( ts.tv_sec, 1 );
  T_eq_u64( ts.tv_nsec, 0 );

  /*
   * Check the initial CLOCK_MONOTONIC in binary time format.
   */
  rtems_clock_get_monotonic_bintime( &bt );
  T_eq_i64( bt.sec, 1 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in binary time format.
   */
  rtems_clock_get_monotonic_bintime( &bt );
  T_eq_i64( bt.sec, 1 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check the initial CLOCK_MONOTONIC in coarse resolution in binary time
   * format.
   */
  rtems_clock_get_monotonic_coarse_bintime( &bt );
  T_eq_i64( bt.sec, 1 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in coarse resolution in binary time
   * format.
   */
  rtems_clock_get_monotonic_coarse_bintime( &bt );
  T_eq_i64( bt.sec, 1 );
  T_eq_u64( bt.frac, 0 );

  /*
   * Check the initial CLOCK_MONOTONIC in signed binary time format.
   */
  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_i64( sb, SBT_1S );

  /*
   * Check that CLOCK_MONOTONIC is frozen in signed binary time format.
   */
  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_i64( sb, SBT_1S );

  /*
   * Check the initial CLOCK_MONOTONIC in seconds and microseconds format.
   */
  rtems_clock_get_monotonic_timeval( &tv );
  T_eq_i64( tv.tv_sec, 1 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in seconds and microseconds format.
   */
  rtems_clock_get_monotonic_timeval( &tv );
  T_eq_i64( tv.tv_sec, 1 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check the initial CLOCK_MONOTONIC in coarse resolution in seconds and
   * microseconds format.
   */
  rtems_clock_get_monotonic_coarse_timeval( &tv );
  T_eq_i64( tv.tv_sec, 1 );
  T_eq_long( tv.tv_usec, 0 );

  /*
   * Check that CLOCK_MONOTONIC is frozen in coarse resolution in seconds and
   * microseconds format.
   */
  rtems_clock_get_monotonic_coarse_timeval( &tv );
  T_eq_i64( tv.tv_sec, 1 );
  T_eq_long( tv.tv_usec, 0 );
}

/**
 * @brief Install timecounter of different quality levels and frequencies.
 */
static void ScoreTimecounterValInstall_Action_3( void )
{
  Timecounter *hqlf;
  Timecounter *hqnf;
  Timecounter *hqhf;
  Timecounter *lq;
  sbintime_t   sb;

  hqlf = &high_quality_low_frequency;
  hqnf = &high_quality_normal_frequency;
  hqhf = &high_quality_high_frequency;
  lq = &low_quality;

  /*
   * Install a timecounter with a high quality level and normal frequency.
   * Check that it was installed.
   */
  hqnf->base.tc_get_timecount = GetTimecount;
  hqnf->base.tc_counter_mask = 0xffffffff;
  hqnf->base.tc_frequency = 0x20000000;
  hqnf->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
  rtems_timecounter_install( &hqnf->base );

  T_eq_u32( GetCounter( hqnf ), 1 );

  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_u32( GetCounter( hqnf ), 2 );
  T_eq_i64( sb, SBT_1S + 8 );

  /*
   * Install a timecounter with a high quality level and low frequency. Check
   * that it was not installed.
   */
  hqlf->base.tc_get_timecount = GetTimecount;
  hqlf->base.tc_counter_mask = 0xffffffff;
  hqlf->base.tc_frequency = 0x10000000;
  hqlf->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
  rtems_timecounter_install( &hqlf->base );

  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 2 );

  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 3 );
  T_eq_i64( sb, SBT_1S + 16 );

  /*
   * Install a timecounter with a high quality level and high frequency. Check
   * that it was installed.
   */
  hqhf->base.tc_get_timecount = GetTimecount;
  hqhf->base.tc_counter_mask = 0xffffffff;
  hqhf->base.tc_frequency = 0x40000000;
  hqhf->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
  rtems_timecounter_install( &hqhf->base );

  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 4 );
  T_eq_u32( GetCounter( hqhf ), 1 );

  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 4 );
  T_eq_u32( GetCounter( hqhf ), 2 );
  T_eq_i64( sb, SBT_1S + 28 );

  /*
   * Install a timecounter with a low quality level.  Check that it was not
   * installed.
   */
  lq->base.tc_get_timecount = GetTimecount;
  lq->base.tc_counter_mask = 0xffffffff;
  lq->base.tc_frequency = 0x80000000;
  lq->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install( &lq->base );

  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 4 );
  T_eq_u32( GetCounter( hqhf ), 2 );
  T_eq_u32( GetCounter( lq ), 0 );

  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_u32( GetCounter( hqlf ), 0 );
  T_eq_u32( GetCounter( hqnf ), 4 );
  T_eq_u32( GetCounter( hqhf ), 3 );
  T_eq_u32( GetCounter( lq ), 0 );
  T_eq_i64( sb, SBT_1S + 32 );
}

/**
 * @brief Call the directives to get the time in the highest resolution
 *   available to the system.
 */
static void ScoreTimecounterValInstall_Action_4( void )
{
  Timecounter    *tc;
  uint32_t        counter;
  struct bintime  bt;
  struct timespec ts;
  struct timeval  tv;

  tc = &high_quality_high_frequency;
  counter = GetCounter( tc );

  /*
   * Check that the timecounter was used by rtems_clock_get_realtime().
   */
  rtems_clock_get_realtime( &ts );
  T_eq_u32( GetCounter( tc ), counter + 1 );

  /*
   * Check that the timecounter was used by rtems_clock_get_realtime_bintime().
   */
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_u32( GetCounter( tc ), counter + 2 );

  /*
   * Check that the timecounter was used by rtems_clock_get_realtime_timeval().
   */
  rtems_clock_get_realtime_timeval( &tv );
  T_eq_u32( GetCounter( tc ), counter + 3 );

  /*
   * Check that the timecounter was used by rtems_clock_get_monotonic().
   */
  rtems_clock_get_monotonic( &ts );
  T_eq_u32( GetCounter( tc ), counter + 4 );

  /*
   * Check that the timecounter was used by
   * rtems_clock_get_monotonic_bintime().
   */
  rtems_clock_get_monotonic_bintime( &bt );
  T_eq_u32( GetCounter( tc ), counter + 5 );

  /*
   * Check that the timecounter was used by
   * rtems_clock_get_monotonic_sbintime().
   */
  (void) rtems_clock_get_monotonic_sbintime();
  T_eq_u32( GetCounter( tc ), counter + 6 );

  /*
   * Check that the timecounter was used by
   * rtems_clock_get_monotonic_timeval().
   */
  rtems_clock_get_monotonic_timeval( &tv );
  T_eq_u32( GetCounter( tc ), counter + 7 );
}

/**
 * @brief Call the directives to get the time in a coarse resolution.
 */
static void ScoreTimecounterValInstall_Action_5( void )
{
  Timecounter    *tc;
  uint32_t        counter;
  struct bintime  bt;
  struct timespec ts;
  struct timeval  tv;

  tc = &high_quality_high_frequency;
  counter = GetCounter( tc );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_realtime_coarse().
   */
  rtems_clock_get_realtime_coarse( &ts );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_realtime_coarse_bintime().
   */
  rtems_clock_get_realtime_coarse_bintime( &bt );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_realtime_coarse_timeval().
   */
  rtems_clock_get_realtime_coarse_timeval( &tv );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_monotonic_coarse().
   */
  rtems_clock_get_monotonic_coarse( &ts );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_monotonic_coarse_bintime().
   */
  rtems_clock_get_monotonic_coarse_bintime( &bt );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_monotonic_coarse_timeval().
   */
  rtems_clock_get_monotonic_coarse_timeval( &tv );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by rtems_clock_get_boot_time().
   */
  rtems_clock_get_boot_time( &ts );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_boot_time_bintime().
   */
  rtems_clock_get_boot_time_bintime( &bt );
  T_eq_u32( GetCounter( tc ), counter );

  /*
   * Check that the timecounter was not used by
   * rtems_clock_get_boot_time_timeval().
   */
  rtems_clock_get_boot_time_timeval( &tv );
  T_eq_u32( GetCounter( tc ), counter );
}

/**
 * @brief Call the directives to get the time in the highest resolution
 *   available to the system.
 */
static void ScoreTimecounterValInstall_Action_6( void )
{
  Timecounter    *tc;
  uint32_t        counter;
  struct bintime  bt;
  sbintime_t      sb;
  struct timespec ts;
  struct timeval  tv;

  tc = &high_quality_high_frequency;
  counter = 3 * tc->base.tc_frequency + 123456789;

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_realtime() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_realtime( &ts );
  T_eq_i64( ts.tv_sec, 567993603 );
  T_eq_u64( ts.tv_nsec, 114978100 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_realtime_bintime() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993603 );
  T_eq_u64( bt.frac, 2120971587975905280 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_realtime_timeval() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_realtime_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993603 );
  T_eq_long( tv.tv_usec, 114978 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_monotonic() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_monotonic( &ts );
  T_eq_i64( ts.tv_sec, 4 );
  T_eq_u64( ts.tv_nsec, 114978100 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_monotonic_bintime() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_monotonic_bintime( &bt );
  T_eq_i64( bt.sec, 4 );
  T_eq_u64( bt.frac, 2120971587975905280 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_monotonic_sbintime() returns the correct time.
   */
  SetCounter( tc, counter );
  sb = rtems_clock_get_monotonic_sbintime();
  T_eq_i64( sb, 17673696364 );

  /*
   * Prepare the timecounter to get a large time difference.  Check that
   * rtems_clock_get_monotonic_timeval() returns the correct time.
   */
  SetCounter( tc, counter );
  rtems_clock_get_monotonic_timeval( &tv );
  T_eq_i64( tv.tv_sec, 4 );
  T_eq_long( tv.tv_usec, 114978 );
}

/**
 * @brief Update the oldest timehand after a large time interval.
 */
static void ScoreTimecounterValInstall_Action_7( void )
{
  Timecounter    *tc;
  struct bintime  bt;

  tc = &high_quality_high_frequency;

  SetCounter( tc, 0 );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993600 );
  T_eq_u64( bt.frac, 103079215104 );

  SetCounter( tc, 2 * tc->base.tc_frequency );
  CallTimecounterTick();

  SetCounter( tc, 2 * tc->base.tc_frequency );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993602 );
  T_eq_u64( bt.frac, 103079215104 );
}

/**
 * @brief Call the simple timecounter tick service with non-zero delta and
 *   offset parameter values so that exactly one second passed.
 */
static void ScoreTimecounterValInstall_Action_8( void )
{
  ISR_lock_Context lock_context;
  Timecounter     *tc;
  struct bintime   bt;

  tc = &high_quality_high_frequency;

  _Timecounter_Acquire( &lock_context );
  _Timecounter_Tick_simple(
    tc->base.tc_frequency / 2,
    GetCounter( tc ) - tc->base.tc_frequency / 2,
    &lock_context
  );

  /*
   * Check that exactly one second passed due to the simple clock tick service.
   */
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993603 );
  T_eq_u64( bt.frac, 103079215104 );
}

/**
 * @brief Install a very high quality timecounter with a low frequency to test
 *   the NTP support.
 */
static void ScoreTimecounterValInstall_Action_9( void )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = &very_high_quality;
  tc->base.tc_get_timecount = GetTimecount;
  tc->base.tc_counter_mask = 0xffffffff;
  tc->base.tc_frequency = 0x01000000;
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 2;
  rtems_timecounter_install( &tc->base );

  T_eq_u32( GetCounter( tc ), 1 );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993603 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME not change. Check that the NTP
   * update second handler is not called.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateCounter );
  SetCounter( tc, tc->base.tc_frequency / 2 );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  T_eq_u32( ntp_counter, 0 );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993603 );
  T_eq_u64( bt.frac, UINT64_C( 9223373256625487872 ) );

  /*
   * Let the seconds value of CLOCK_REALTIME change by one. Check that the NTP
   * update second handler is called exactly once.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateCounter );
  SetCounter( tc, tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  T_eq_u32( ntp_counter, 1 );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993604 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by 200. Check that the NTP
   * update second handler is called exactly 200 times.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateCounter );
  SetCounter( tc, 201 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  T_eq_u32( ntp_counter, 201 );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993804 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by 201. Check that the NTP
   * update second handler is called exactly twice.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateCounter );
  SetCounter( tc, 402 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  T_eq_u32( ntp_counter, 203 );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567994005 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by one. Check that the NTP
   * update second handler is incremented the CLOCK_REALTIME by one second.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateSecondIncrement );
  SetCounter( tc, 403 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567994007 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by one. Check that the NTP
   * update second handler is decremented the CLOCK_REALTIME by one second.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateSecondDecrement );
  SetCounter( tc, 404 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567994007 );
  T_eq_u64( bt.frac, 1219770712064 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by one. Check that the NTP
   * update second handler increased the timecounter frequency.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateAdjustmentFaster );
  SetCounter( tc, 405 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  SetCounter( tc, 406 * tc->base.tc_frequency );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567994009 );
  T_eq_u64( bt.frac, 92353004044288 );

  /*
   * Let the seconds value of CLOCK_REALTIME change by one. Check that the NTP
   * update second handler decreased the timecounter frequency.
   */
  _Timecounter_Set_NTP_update_second( NtpUpdateAdjustmentSlower );
  SetCounter( tc, 407 * tc->base.tc_frequency );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  SetCounter( tc, 408 * tc->base.tc_frequency );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567994011 );
  T_eq_u64( bt.frac, 92353004044288 );
}

/**
 * @fn void T_case_body_ScoreTimecounterValInstall( void )
 */
T_TEST_CASE( ScoreTimecounterValInstall )
{
  ScoreTimecounterValInstall_Action_0();
  ScoreTimecounterValInstall_Action_1();
  ScoreTimecounterValInstall_Action_2();
  ScoreTimecounterValInstall_Action_3();
  ScoreTimecounterValInstall_Action_4();
  ScoreTimecounterValInstall_Action_5();
  ScoreTimecounterValInstall_Action_6();
  ScoreTimecounterValInstall_Action_7();
  ScoreTimecounterValInstall_Action_8();
  ScoreTimecounterValInstall_Action_9();
}

/** @} */
