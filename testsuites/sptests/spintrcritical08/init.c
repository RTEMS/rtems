/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 1989-2012.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems/score/watchdogimpl.h>
#include <rtems/rtems/ratemonimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 8";

typedef struct {
  rtems_id        period;
  Thread_Control *thread;
} test_context;

static rtems_rate_monotonic_period_states getState(test_context *ctx)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;

  the_period = _Rate_monotonic_Get( ctx->period, &lock_context );
  T_quiet_assert_not_null( the_period );
  _ISR_lock_ISR_enable( &lock_context );

  return the_period->state;
}

static T_interrupt_test_state interrupt( void *arg )
{
  test_context                       *ctx;
  Per_CPU_Control                    *cpu_self;
  Watchdog_Header                    *header;
  Watchdog_Control                   *watchdog;
  T_interrupt_test_state              state;
  Thread_Wait_flags                   flags;
  ISR_Level                           level;
  rtems_rate_monotonic_period_states  previous_period_state;

  state = T_interrupt_test_get_state();

  if ( state != T_INTERRUPT_TEST_ACTION ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  ctx = arg;
  cpu_self = _Per_CPU_Get();
  header = &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  watchdog = (Watchdog_Control *) header->first;
  T_quiet_assert_not_null( watchdog );
  T_quiet_eq_u64( watchdog->expire, cpu_self->Watchdog.ticks );
  T_quiet_eq_ptr( watchdog->routine, _Rate_monotonic_Timeout );

  flags = _Thread_Wait_flags_get( ctx->thread );

  _ISR_Local_disable( level );
  _Watchdog_Per_CPU_remove( watchdog, cpu_self, header );
  _ISR_Local_enable( level );

  previous_period_state = getState( ctx );
  ( *watchdog->routine )( watchdog );

  if ( flags == RATE_MONOTONIC_INTEND_TO_BLOCK ) {
    T_quiet_eq_int( previous_period_state, RATE_MONOTONIC_ACTIVE );
    T_quiet_eq_int( getState( ctx ), RATE_MONOTONIC_ACTIVE );
    state = T_INTERRUPT_TEST_DONE;
  } else if ( flags == THREAD_WAIT_STATE_READY ) {
    T_quiet_true(
      previous_period_state == RATE_MONOTONIC_ACTIVE
        || previous_period_state == RATE_MONOTONIC_EXPIRED
    );
    state = T_INTERRUPT_TEST_EARLY;
  } else {
    T_quiet_eq_int( flags, RATE_MONOTONIC_BLOCKED );
    T_quiet_true(
      previous_period_state == RATE_MONOTONIC_ACTIVE
        || previous_period_state == RATE_MONOTONIC_EXPIRED
    );
    state = T_INTERRUPT_TEST_LATE;
  }

  return state;
}

static void prepare( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;

  do {
    sc = rtems_rate_monotonic_cancel( ctx->period );
    T_quiet_rsc_success( sc );

    sc = rtems_rate_monotonic_period( ctx->period, 1 );
    T_quiet_rsc_success( sc );

    /*
     * Depending on the time to the next clock tick and the CPU time available
     * to a simulator, we may get sporadic RTEMS_TIMEOUT here.  In the next
     * round we are synchronized with the clock tick.
     */
    sc = rtems_rate_monotonic_period( ctx->period, 1 );
  } while ( sc != RTEMS_SUCCESSFUL );
}

static void action( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;

  sc = rtems_rate_monotonic_period( ctx->period, 1 );
  T_quiet_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT );

  T_interrupt_test_busy_wait_for_interrupt();
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE( RateMonotonicPeriodInterrupt )
{
  test_context ctx;
  rtems_status_code sc;
  T_interrupt_test_state state;

  memset( &ctx, 0 , sizeof( ctx ) );
  ctx.thread = _Thread_Get_executing();

  sc = rtems_rate_monotonic_create(
    rtems_build_name( 'P', 'E', 'R', '1' ),
    &ctx.period
  );
  T_rsc_success( sc );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_rate_monotonic_delete( ctx.period );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_PERIODS     1

#define CONFIGURE_MICROSECONDS_PER_TICK  10000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
