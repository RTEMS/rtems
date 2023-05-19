/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

#if defined(FIRE_AFTER)
  #define TEST_NAME          "13"
  #define TEST_STRING        TimerFireAfterInterrupt
  #define TEST_DIRECTIVE     rtems_timer_fire_after

#elif defined(SERVER_FIRE_AFTER)
  #define TEST_NAME          "14"
  #define TEST_STRING        TimerServerFireAfterInterrupt
  #define TEST_DIRECTIVE     rtems_timer_server_fire_after

#else
  #error "Test Mode not defined"
#endif

const char rtems_test_name[] = "SPINTRCRITICAL " TEST_NAME;

typedef struct {
  rtems_id timer;
  long potential_hits;
  volatile bool early;
  volatile bool late;
} test_context;

static rtems_timer_service_routine TimerMethod(
  rtems_id  timer,
  void     *arg
)
{
  (void) timer;
  (void) arg;
}

static T_interrupt_test_state interrupt( void *arg )
{
  test_context           *ctx;
  rtems_status_code       sc;
  T_interrupt_test_state  state;

  state = T_interrupt_test_get_state();

  if ( state != T_INTERRUPT_TEST_ACTION ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  ctx = arg;
  sc = TEST_DIRECTIVE( ctx->timer, 10, TimerMethod, NULL );
  T_quiet_rsc_success( sc );

  if ( ctx->early ) {
    state = T_INTERRUPT_TEST_EARLY;
  } else if ( ctx->late ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    ++ctx->potential_hits;

    if ( ctx->potential_hits > 13 ) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_CONTINUE;
    }
  }

  return state;
}

static void prepare( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->early = true;
  ctx->late = false;
}

static void action( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;
  ctx->early = false;
  sc = TEST_DIRECTIVE( ctx->timer, 10, TimerMethod, NULL );
  T_quiet_rsc_success( sc );
  ctx->late = true;

  T_interrupt_test_busy_wait_for_interrupt();
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE( TEST_STRING )
{
  test_context           ctx;
  rtems_status_code      sc;
  T_interrupt_test_state state;

  memset( &ctx, 0, sizeof( ctx ) );

  sc = rtems_timer_create(
    rtems_build_name( 'P', 'E', 'R', '1' ),
    &ctx.timer
  );
  T_assert_rsc_success( sc );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_timer_delete( ctx.timer );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
#if defined(SERVER_FIRE_AFTER)
  (void) rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
#endif

  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#if defined(SERVER_FIRE_AFTER)
  #define CONFIGURE_MAXIMUM_TASKS     3
#else
  #define CONFIGURE_MAXIMUM_TASKS     2
#endif
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
