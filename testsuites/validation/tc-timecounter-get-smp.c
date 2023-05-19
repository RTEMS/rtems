/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTimecounterValGetSmp
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
#include <rtems/score/smpbarrier.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTimecounterValGetSmp spec:/score/timecounter/val/get-smp
 *
 * @ingroup TestsuitesValidationTimecounterSmp0
 *
 * @brief Tests directives to get a time value.
 *
 * This test case performs the following actions:
 *
 * - Install timecounter of different quality levels and frequencies.
 *
 *   - Call the rtems_clock_get_realtime() directive and let it observe a
 *     generation number of zero as well as a generation number change.
 *
 *   - Call the rtems_clock_get_realtime_bintime() directive and let it observe
 *     a generation number of zero as well as a generation number change.
 *
 *   - Call the rtems_clock_get_realtime_timeval() directive and let it observe
 *     a generation number of zero as well as a generation number change.
 *
 *   - Call the rtems_clock_get_monotonic() directive and let it observe a
 *     generation number of zero as well as a generation number change.
 *
 *   - Call the rtems_clock_get_monotonic_bintime() directive and let it
 *     observe a generation number of zero as well as a generation number
 *     change.
 *
 *   - Call the rtems_clock_get_monotonic_sbintime() directive and let it
 *     observe a generation number of zero as well as a generation number
 *     change.
 *
 *   - Call the rtems_clock_get_monotonic_timeval() directive and let it
 *     observe a generation number of zero as well as a generation number
 *     change.
 *
 *   - Delete the synchronous worker task.  Reinitialize the barrier and
 *     barrier states.  Start the zero worker task.
 *
 *   - Call the rtems_clock_get_realtime_coarse() directive and try to let it
 *     observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_realtime_coarse_bintime() directive and try to
 *     let it observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_realtime_coarse_timeval() directive and try to
 *     let it observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_monotonic_coarse() directive and try to let it
 *     observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_monotonic_coarse_bintime() directive and try to
 *     let it observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_monotonic_coarse_timeval() directive and try to
 *     let it observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_boot_time() directive and try to let it observe
 *     a generation number of zero.
 *
 *   - Call the rtems_clock_get_boot_time_bintime() directive and try to let it
 *     observe a generation number of zero.
 *
 *   - Call the rtems_clock_get_boot_time_timeval() directive and try to let it
 *     observe a generation number of zero.
 *
 *   - Delete the zero worker task.  Reinitialize the barrier and barrier
 *     states.  Start the change worker task.
 *
 *   - Call the rtems_clock_get_realtime_coarse() directive and try to let it
 *     observe a changing generation number.
 *
 *   - Call the rtems_clock_get_realtime_coarse_bintime() directive and try to
 *     let it observe a changing generation number.
 *
 *   - Call the rtems_clock_get_realtime_coarse_timeval() directive and try to
 *     let it observe a changing generation number.
 *
 *   - Call the rtems_clock_get_monotonic_coarse() directive and try to let it
 *     observe a changing generation number.
 *
 *   - Call the rtems_clock_get_monotonic_coarse_bintime() directive and try to
 *     let it observe a changing generation number.
 *
 *   - Call the rtems_clock_get_monotonic_coarse_timeval() directive and try to
 *     let it observe a changing generation number.
 *
 *   - Call the rtems_clock_get_boot_time() directive and try to let it observe
 *     a changing generation number.
 *
 *   - Call the rtems_clock_get_boot_time_bintime() directive and try to let it
 *     observe a changing generation number.
 *
 *   - Call the rtems_clock_get_boot_time_timeval() directive and try to let it
 *     observe a changing generation number.
 *
 *   - Delete the change worker task.
 *
 * @{
 */

typedef struct {
  struct timecounter  base;
  Atomic_Ulong        counter;
  Atomic_Uint        *generation_0;
  Atomic_Uint        *generation_1;
  SMP_barrier_Control barrier;
  SMP_barrier_State   barrier_state[ 2 ];
} Timecounter;

static Timecounter test_timecounter;

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

static uint32_t GetTimecountBarrier( struct timecounter *base )
{
  Timecounter *tc;

  tc = (Timecounter *) base;

  /* C0, C1, C2 */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  /* D0, D1, D2 */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  return GetTimecount( &tc->base );
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

static void CallTimecounterTick( void )
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  rtems_timecounter_tick();
  _Thread_Dispatch_enable( cpu_self );
}

static void SetGeneration( Timecounter *tc, unsigned int generation )
{
  _Atomic_Store_uint( tc->generation_0, generation, ATOMIC_ORDER_RELAXED );
  _Atomic_Store_uint( tc->generation_1, generation, ATOMIC_ORDER_RELAXED );
}

static void PrepareSynchronousWork( Timecounter *tc )
{
  /* A */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  SetCounter( tc, 0 );

  /* B */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );
}

static void CleanupSynchronousWork( Timecounter *tc )
{
  /* E */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  T_eq_u32( GetCounter( tc ), 3 );
}

static void SynchronousWorker( rtems_task_argument arg )
{
  Timecounter *tc;

  tc = (Timecounter *) arg;

  while ( true ) {
    /* A */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    SetGeneration( tc, 0 );

    /* B */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* C0 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    SetGeneration( tc, 1 );

    /* D0 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* C1 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    SetGeneration( tc, 2 );

    /* D1 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* C2 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* D2 */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* E */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );
  }
}

static void PrepareZeroWork( Timecounter *tc )
{
  /* F */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  SetCounter( tc, 0 );

  /* G */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );
}

static void CleanupZeroWork( Timecounter *tc )
{
  /* H */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  T_eq_u32( GetCounter( tc ), 0 );
}

static void ZeroWorker( rtems_task_argument arg )
{
  Timecounter *tc;

  tc = (Timecounter *) arg;

  while ( true ) {
    /* F */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    SetGeneration( tc, 0 );

    /* G */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    rtems_counter_delay_nanoseconds( 10000000 );
    SetGeneration( tc, 1 );

    /* H */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );
  }
}

static void PrepareChangeWork( Timecounter *tc )
{
  /* F */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  SetCounter( tc, 0 );

  /* G */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );
}

static void CleanupChangeWork( Timecounter *tc )
{
  /* H */
  _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 0 ], 2 );

  T_eq_u32( GetCounter( tc ), 0 );
}

static void ChangeWorker( rtems_task_argument arg )
{
  Timecounter *tc;

  tc = (Timecounter *) arg;

  while ( true ) {
    unsigned int i;

    /* F */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    /* G */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );

    for ( i = 1; i < 1000; ++i ) {
      SetGeneration( tc, i );
    }

    /* H */
    _SMP_barrier_Wait( &tc->barrier, &tc->barrier_state[ 1 ], 2 );
  }
}

/* This definition must be identical to the one in kern_tc.c */
struct timehands {
  struct timecounter *th_counter;
  int64_t             th_adjustment;
  uint64_t            th_scale;
  uint32_t            th_large_delta;
  uint32_t            th_offset_count;
  struct bintime      th_offset;
  struct bintime      th_bintime;
  struct timeval      th_microtime;
  struct timespec     th_nanotime;
  struct bintime      th_boottime;
  Atomic_Uint         th_generation;
  struct timehands   *th_next;
};

static void NtpUpdateSecond( int64_t *adjustment, time_t *newsec )
{
  Timecounter      *tc;
  struct timehands *th;

  tc = &test_timecounter;
  th = RTEMS_CONTAINER_OF( adjustment, struct timehands, th_adjustment );
  T_assert_eq_ptr( th, th->th_next->th_next );
  tc->generation_0 = &th->th_generation;
  tc->generation_1 = &th->th_next->th_generation;
}

/**
 * @brief Install timecounter of different quality levels and frequencies.
 */
static void ScoreTimecounterValGetSmp_Action_0( void )
{
  Timecounter     *tc;
  rtems_id         worker_id;
  struct bintime   bt;
  sbintime_t       sbt;
  struct timespec  ts;
  struct timeval   tv;
  unsigned int     i;

  tc = &test_timecounter;
  tc->base.tc_get_timecount = GetTimecount;
  tc->base.tc_counter_mask = 0xffffffff;
  tc->base.tc_frequency = 0x10000000;
  tc->base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER;
  rtems_timecounter_install( &tc->base );

  SetCounter( tc, tc->base.tc_frequency );
  _Timecounter_Set_NTP_update_second( NtpUpdateSecond );
  CallTimecounterTick();
  _Timecounter_Set_NTP_update_second( NULL );

  T_assert_not_null( tc->generation_0 );
  T_assert_not_null( tc->generation_1 );

  _SMP_barrier_Control_initialize( &tc->barrier );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 0 ] );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 1 ] );

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( worker_id, SynchronousWorker, tc );

  tc->base.tc_get_timecount = GetTimecountBarrier;

  /*
   * Call the rtems_clock_get_realtime() directive and let it observe a
   * generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_realtime( &ts );
  T_eq_i64( ts.tv_sec, 567993616 );
  T_eq_long( ts.tv_nsec, 7 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_realtime_bintime() directive and let it observe a
   * generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_realtime_bintime( &bt );
  T_eq_i64( bt.sec, 567993616 );
  T_eq_u64( bt.frac, 137438953472 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_realtime_timeval() directive and let it observe a
   * generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_realtime_timeval( &tv );
  T_eq_i64( tv.tv_sec, 567993616 );
  T_eq_long( tv.tv_usec, 0 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_monotonic() directive and let it observe a
   * generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_monotonic( &ts );
  T_eq_i64( ts.tv_sec, 17 );
  T_eq_long( ts.tv_nsec, 7 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_bintime() directive and let it observe
   * a generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_monotonic_bintime( &bt );
  T_eq_i64( bt.sec, 17 );
  T_eq_u64( bt.frac, 137438953472 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_sbintime() directive and let it observe
   * a generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  sbt = rtems_clock_get_monotonic_sbintime();
  T_eq_i64( sbt, 73014444064 );
  CleanupSynchronousWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_timeval() directive and let it observe
   * a generation number of zero as well as a generation number change.
   */
  PrepareSynchronousWork( tc );
  rtems_clock_get_monotonic_timeval( &tv );
  T_eq_i64( tv.tv_sec, 17 );
  T_eq_long( tv.tv_usec, 0 );
  CleanupSynchronousWork( tc );

  /*
   * Delete the synchronous worker task.  Reinitialize the barrier and barrier
   * states.  Start the zero worker task.
   */
  tc->base.tc_get_timecount = GetTimecount;
  DeleteTask( worker_id );

  _SMP_barrier_Control_initialize( &tc->barrier );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 0 ] );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 1 ] );

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( worker_id, ZeroWorker, tc );

  /*
   * Call the rtems_clock_get_realtime_coarse() directive and try to let it
   * observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_realtime_coarse( &ts );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_realtime_coarse_bintime() directive and try to
   * let it observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_realtime_coarse_bintime( &bt );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_realtime_coarse_timeval() directive and try to
   * let it observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_realtime_coarse_timeval( &tv );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse() directive and try to let it
   * observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_monotonic_coarse( &ts );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse_bintime() directive and try to
   * let it observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_monotonic_coarse_bintime( &bt );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse_timeval() directive and try to
   * let it observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_monotonic_coarse_timeval( &tv );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_boot_time() directive and try to let it observe a
   * generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_boot_time( &ts );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_boot_time_bintime() directive and try to let it
   * observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_boot_time_bintime( &bt );
  CleanupZeroWork( tc );

  /*
   * Call the rtems_clock_get_boot_time_timeval() directive and try to let it
   * observe a generation number of zero.
   */
  PrepareZeroWork( tc );
  rtems_clock_get_boot_time_timeval( &tv );
  CleanupZeroWork( tc );

  /*
   * Delete the zero worker task.  Reinitialize the barrier and barrier states.
   * Start the change worker task.
   */
  DeleteTask( worker_id );

  _SMP_barrier_Control_initialize( &tc->barrier );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 0 ] );
  _SMP_barrier_State_initialize( &tc->barrier_state[ 1 ] );

  worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( worker_id, ChangeWorker, tc );

  /*
   * Call the rtems_clock_get_realtime_coarse() directive and try to let it
   * observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_realtime_coarse( &ts );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_realtime_coarse_bintime() directive and try to
   * let it observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_realtime_coarse_bintime( &bt );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_realtime_coarse_timeval() directive and try to
   * let it observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_realtime_coarse_timeval( &tv );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse() directive and try to let it
   * observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_monotonic_coarse( &ts );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse_bintime() directive and try to
   * let it observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_monotonic_coarse_bintime( &bt );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_monotonic_coarse_timeval() directive and try to
   * let it observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_monotonic_coarse_timeval( &tv );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_boot_time() directive and try to let it observe a
   * changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_boot_time( &ts );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_boot_time_bintime() directive and try to let it
   * observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_boot_time_bintime( &bt );
  }

  CleanupChangeWork( tc );

  /*
   * Call the rtems_clock_get_boot_time_timeval() directive and try to let it
   * observe a changing generation number.
   */
  PrepareChangeWork( tc );

  for ( i = 0; i < 100; ++i ) {
    rtems_clock_get_boot_time_timeval( &tv );
  }

  CleanupChangeWork( tc );

  /*
   * Delete the change worker task.
   */
  DeleteTask( worker_id );
}

/**
 * @fn void T_case_body_ScoreTimecounterValGetSmp( void )
 */
T_TEST_CASE( ScoreTimecounterValGetSmp )
{
  ScoreTimecounterValGetSmp_Action_0();
}

/** @} */
