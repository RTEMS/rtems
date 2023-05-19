/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2012, 2020 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>
#include <rtems/test-info.h>

const char rtems_test_name[] = "SPINTRCRITICAL 18";

#define WAKE_UP RTEMS_EVENT_0

#define PRIORITY_LOW 3

#define PRIORITY_MIDDLE 2

#define PRIORITY_HIGH 1

typedef struct {
  rtems_id middle_priority_task;
  rtems_id high_priority_task;
  bool high_priority_task_activated;
  volatile bool early;
  volatile bool switching;
  volatile bool late;
  long potential_hits;
} test_context;

static void wake_up( rtems_id task )
{
  rtems_status_code sc;

  sc = rtems_event_send( task, WAKE_UP );
  T_quiet_rsc_success( sc );
}

static void wait_for_wake_up( void )
{
  rtems_status_code sc;
  rtems_event_set events;

  sc = rtems_event_receive(
    WAKE_UP,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  T_quiet_rsc_success( sc );
  T_quiet_eq_u32( events, WAKE_UP );
}

static T_interrupt_test_state active_high_priority_task( void *arg )
{
  test_context *ctx = arg;
  T_interrupt_test_state state;

  state = T_interrupt_test_get_state();

  if ( state != T_INTERRUPT_TEST_ACTION ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  T_quiet_false( ctx->high_priority_task_activated );
  ctx->high_priority_task_activated = true;
  wake_up( ctx->high_priority_task );

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

static void middle_priority_task( rtems_task_argument arg )
{
  test_context *ctx = (test_context *) arg;

  while ( true ) {
    wait_for_wake_up();
    ctx->late = true;

    T_quiet_false( ctx->high_priority_task_activated );
  }
}

static void high_priority_task( rtems_task_argument arg )
{
  test_context *ctx = (test_context *) arg;

  while ( true ) {
    wait_for_wake_up();

    T_quiet_true( ctx->high_priority_task_activated );
    ctx->high_priority_task_activated = false;
  }
}

static void prepare( void *arg )
{
  test_context *ctx = arg;

  ctx->early = true;
  ctx->switching = false;
  ctx->late = false;
}

static void action( void *arg )
{
  test_context *ctx = arg;

  ctx->early = false;
  wake_up( ctx->middle_priority_task );
}

static void blocked( void *arg )
{
  test_context *ctx = arg;
  T_interrupt_test_state state;

  state = T_interrupt_test_change_state(
    T_INTERRUPT_TEST_ACTION,
    T_INTERRUPT_TEST_LATE
  );

  if ( state == T_INTERRUPT_TEST_ACTION ) {
    ctx->switching = true;
    T_busy( 100 );
    ctx->switching = false;
  }
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = active_high_priority_task,
  .blocked = blocked,
  .max_iteration_count = 10000
};

T_TEST_CASE(InterruptDuringThreadDispatch)
{
  T_interrupt_test_state state;
  test_context ctx;
  rtems_status_code sc;

  memset( &ctx, 0, sizeof( ctx ) );

  sc = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    PRIORITY_HIGH,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx.high_priority_task
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_start(
    ctx.high_priority_task,
    high_priority_task,
    (rtems_task_argument) &ctx
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_create(
    rtems_build_name( 'M', 'I', 'D', 'L' ),
    PRIORITY_MIDDLE,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx.middle_priority_task
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_start(
    ctx.middle_priority_task,
    middle_priority_task,
    (rtems_task_argument) &ctx
  );
  T_assert_rsc_success( sc );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_task_delete( ctx.high_priority_task );
  T_rsc_success( sc );

  sc = rtems_task_delete( ctx.middle_priority_task );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 3

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_LOW
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
