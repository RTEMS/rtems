/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTimecounterValGet
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
#include <rtems/counter.h>
#include <rtems/timecounter.h>
#include <rtems/score/timecounterimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTimecounterValGet spec:/score/timecounter/val/get
 *
 * @ingroup TestsuitesValidationTimecounter1
 *
 * @brief Tests directives to get a time value.
 *
 * This test case performs the following actions:
 *
 * - Install a timecounter which can be used to perform interrut tests for the
 *   get time directives.
 *
 *   - Try to interrupt the rtems_clock_get_realtime() directive to provoke a
 *     change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_realtime_bintime() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_realtime_timeval() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic() directive to provoke a
 *     change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_bintime() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_sbintime() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_timeval() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Prepare for the coarse get time directives.
 *
 *   - Try to interrupt the rtems_clock_get_realtime_coarse() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_realtime_coarse_bintime() directive
 *     to provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_realtime_coarse_timeval() directive
 *     to provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_coarse() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_coarse_bintime()
 *     directive to provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_monotonic_coarse_timeval()
 *     directive to provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_boot_time() directive to provoke a
 *     change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_boot_time_bintime() directive to
 *     provoke a change in the timehand generation number.
 *
 *   - Try to interrupt the rtems_clock_get_boot_time_timeval() directive to
 *     provoke a change in the timehand generation number.
 *
 * @{
 */

typedef enum {
  STATE_EARLY,
  STATE_GET_TIMECOUNT_BEFORE,
  STATE_GET_TIMECOUNT_BUSY,
  STATE_GET_TIMECOUNT_DONE,
  STATE_GET_TIMECOUNT_AFTER
} State;

typedef struct {
  struct timecounter  base;
  State               state;
  uint_fast32_t       busy;
  struct bintime      tod;
} Timecounter;

static Timecounter test_timecounter;

static uint32_t GetTimecount( struct timecounter *base )
{
  Timecounter *tc;

  tc = (Timecounter *) base;

  if (
    tc->state == STATE_GET_TIMECOUNT_BEFORE &&
    !rtems_interrupt_is_in_progress()
  ) {
    tc->state = STATE_GET_TIMECOUNT_BUSY;
    T_busy( tc->busy );
    tc->state = STATE_GET_TIMECOUNT_DONE;
  }

  return rtems_counter_read();
}

static void InterruptPrepare( void *arg )
{
  Timecounter *tc;

  tc = (Timecounter *) arg;
  tc->state = STATE_EARLY;
}

static void ActionRealtime( void *arg )
{
  Timecounter    *tc;
  struct timespec ts;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime( &ts );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionRealtimeBintime( void *arg )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime_bintime( &bt );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionRealtimeTimeval( void *arg )
{
  Timecounter   *tc;
  struct timeval tv;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime_timeval( &tv );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionMonotonic( void *arg )
{
  Timecounter    *tc;
  struct timespec ts;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic( &ts );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionMonotonicBintime( void *arg )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic_bintime( &bt );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionMonotonicSbintime( void *arg )
{
  Timecounter *tc;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  (void) rtems_clock_get_monotonic_sbintime();
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionMonotonicTimeval( void *arg )
{
  Timecounter   *tc;
  struct timeval tv;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic_timeval( &tv );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseRealtime( void *arg )
{
  Timecounter    *tc;
  struct timespec ts;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime_coarse( &ts );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseRealtimeBintime( void *arg )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime_coarse_bintime( &bt );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseRealtimeTimeval( void *arg )
{
  Timecounter   *tc;
  struct timeval tv;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_realtime_coarse_timeval( &tv );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseMonotonic( void *arg )
{
  Timecounter    *tc;
  struct timespec ts;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic_coarse( &ts );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseMonotonicBintime( void *arg )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic_coarse_bintime( &bt );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionCoarseMonotonicTimeval( void *arg )
{
  Timecounter   *tc;
  struct timeval tv;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_monotonic_coarse_timeval( &tv );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionBootTime( void *arg )
{
  Timecounter    *tc;
  struct timespec ts;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_boot_time( &ts );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionBootTimeBintime( void *arg )
{
  Timecounter   *tc;
  struct bintime bt;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_boot_time_bintime( &bt );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void ActionBootTimeTimeval( void *arg )
{
  Timecounter   *tc;
  struct timeval tv;

  tc = (Timecounter *) arg;
  tc->state = STATE_GET_TIMECOUNT_BEFORE;
  rtems_clock_get_boot_time_timeval( &tv );
  tc->state = STATE_GET_TIMECOUNT_AFTER;
}

static void CallTimcounterWindupTwice( const Timecounter *tc )
{
  ISR_lock_Context lock_context;

  /*
   * Make sure that tc_windup() was called at least twice to increment the
   * generation number for * both timehands.
   */

  _Timecounter_Acquire( &lock_context );
  _Timecounter_Set_clock( &tc->tod, &lock_context );

  _Timecounter_Acquire( &lock_context );
  _Timecounter_Set_clock( &tc->tod, &lock_context );
}

static T_interrupt_test_state Interrupt( void *arg )
{
  Timecounter *tc;
  State        state;

  tc = (Timecounter *) arg;
  state = tc->state;

  if ( state == STATE_EARLY || state == STATE_GET_TIMECOUNT_BEFORE ) {
    return T_INTERRUPT_TEST_EARLY;
  }

  if ( state == STATE_GET_TIMECOUNT_BUSY ) {
    CallTimcounterWindupTwice( tc );

    return T_INTERRUPT_TEST_DONE;
  }

  return T_INTERRUPT_TEST_LATE;
}

static T_interrupt_test_state InterruptCoarse( void *arg )
{
  Timecounter *tc;
  State        state;

  tc = (Timecounter *) arg;
  state = tc->state;

  if ( state == STATE_EARLY ) {
    return T_INTERRUPT_TEST_EARLY;
  }

  if ( state == STATE_GET_TIMECOUNT_BEFORE ) {
    CallTimcounterWindupTwice( tc );

    return T_INTERRUPT_TEST_DONE;
  }

  return T_INTERRUPT_TEST_LATE;
}

static bool InterruptTest(
  const T_interrupt_test_config *config,
  void                          *arg,
  uint32_t                       iterations
)
{
  uint32_t i;
  bool     ok;

  ok = false;

  for ( i = 0; i < iterations; ++i ) {
    T_interrupt_test_state test_state;

    test_state = T_interrupt_test( config, arg );
    ok = ok || test_state == T_INTERRUPT_TEST_DONE;
  }

  return ok;
}

/**
 * @brief Install a timecounter which can be used to perform interrut tests for
 *   the get time directives.
 */
static void ScoreTimecounterValGet_Action_0( void )
{
  T_interrupt_test_config config = {
    .prepare = InterruptPrepare,
    .interrupt = Interrupt,
    .max_iteration_count = 10000
  };
  Timecounter *tc;

  tc = &test_timecounter;
  tc->base.tc_get_timecount = GetTimecount;
  tc->base.tc_counter_mask = 0xffffffff;
  tc->base.tc_frequency = rtems_counter_frequency();
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
  tc->busy = T_get_one_clock_tick_busy() / 10;
  rtems_clock_get_realtime_bintime( &tc->tod );
  rtems_timecounter_install( &tc->base );

  /*
   * Try to interrupt the rtems_clock_get_realtime() directive to provoke a
   * change in the timehand generation number.
   */
  config.action = ActionRealtime;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_realtime_bintime() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionRealtimeBintime;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_realtime_timeval() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionRealtimeTimeval;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic() directive to provoke a
   * change in the timehand generation number.
   */
  config.action = ActionMonotonic;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_bintime() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionMonotonicBintime;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_sbintime() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionMonotonicSbintime;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_timeval() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionMonotonicTimeval;
  T_true( InterruptTest( &config, tc, 1 ) );

  /*
   * Prepare for the coarse get time directives.
   */
  config.interrupt = InterruptCoarse;

  /*
   * Try to interrupt the rtems_clock_get_realtime_coarse() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseRealtime;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_realtime_coarse_bintime() directive
   * to provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseRealtimeBintime;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_realtime_coarse_timeval() directive
   * to provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseRealtimeTimeval;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_coarse() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseMonotonic;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_coarse_bintime() directive
   * to provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseMonotonicBintime;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_monotonic_coarse_timeval() directive
   * to provoke a change in the timehand generation number.
   */
  config.action = ActionCoarseMonotonicTimeval;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_boot_time() directive to provoke a
   * change in the timehand generation number.
   */
  config.action = ActionBootTime;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_boot_time_bintime() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionBootTimeBintime;
  T_true( InterruptTest( &config, tc, 10 ) );

  /*
   * Try to interrupt the rtems_clock_get_boot_time_timeval() directive to
   * provoke a change in the timehand generation number.
   */
  config.action = ActionBootTimeTimeval;
  T_true( InterruptTest( &config, tc, 10 ) );
}

/**
 * @fn void T_case_body_ScoreTimecounterValGet( void )
 */
T_TEST_CASE( ScoreTimecounterValGet )
{
  ScoreTimecounterValGet_Action_0();
}

/** @} */
