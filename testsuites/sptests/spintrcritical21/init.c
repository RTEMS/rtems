/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Classic API Signal to Task from ISR
 *
 *  Copyright (C) 2020 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 1989-2011.
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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems/score/threadimpl.h>
#include <rtems/rtems/eventimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 21";

#define MAX_ITERATION_COUNT 10000

typedef struct {
  rtems_id        main_task;
  Thread_Control *main_thread;
  rtems_id        other_task;
} test_context;

static void clear_pending_events( void )
{
  rtems_event_set out;

  (void) rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_NO_WAIT | RTEMS_EVENT_ANY,
    0,
    &out
  );
}

static bool is_blocked( Thread_Wait_flags flags )
{
  return flags == ( THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_BLOCKED );
}

static bool interrupts_blocking_op( Thread_Wait_flags flags )
{
  return flags
    == ( THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_INTEND_TO_BLOCK );
}

static T_interrupt_test_state event_from_isr_interrupt(
  void *arg
)
{
  test_context           *ctx;
  T_interrupt_test_state state;
  Thread_Wait_flags      flags;
  rtems_status_code      status;

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->main_thread );

  if ( interrupts_blocking_op( flags ) ) {
    /*
     *  This event send hits the critical section but sends to
     *  another task so doesn't impact this critical section.
     */
    status = rtems_event_send( ctx->other_task, 0x02 );
    T_quiet_rsc_success( status );

    /*
     *  This event send hits the main task but doesn't satisfy
     *  it's blocking condition so it will still block
     */
    status = rtems_event_send( ctx->main_task, 0x02 );
    T_quiet_rsc_success( status );

    state = T_INTERRUPT_TEST_DONE;
  } else if ( is_blocked( flags ) ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  status = rtems_event_send( ctx->main_task, 0x01 );
  T_quiet_rsc_success( status );

  return state;
}

static void event_from_isr_action( void *arg )
{
  rtems_status_code status;
  rtems_event_set   out;

  (void) arg;

  status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 0, &out );
  T_quiet_rsc_success( status );
}

static const T_interrupt_test_config event_from_isr_config = {
  .action = event_from_isr_action,
  .interrupt = event_from_isr_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE(EventFromISR)
{
  test_context           ctx;
  T_interrupt_test_state state;
  rtems_status_code      status;

  ctx.main_task = rtems_task_self();
  ctx.main_thread = _Thread_Get_executing();

  status = rtems_task_create(
    0xa5a5a5a5,
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx.other_task
  );
  T_assert_rsc_success( status );

  state = T_interrupt_test( &event_from_isr_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  status = rtems_task_delete( ctx.other_task );
  T_rsc_success( status );

  clear_pending_events();
}

static T_interrupt_test_state event_with_timeout_from_isr_interrupt(
  void *arg
)
{
  test_context           *ctx;
  T_interrupt_test_state  state;
  Thread_Wait_flags       flags;
  rtems_status_code       status;

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->main_thread );

  if ( interrupts_blocking_op( flags ) ) {
    /*
     *  We want to catch the task while it is blocking.  Otherwise
     *  just send and make it happy.
     */
    state = T_INTERRUPT_TEST_DONE;
  } else if ( is_blocked( flags ) ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  status = rtems_event_send( ctx->main_task, 0x01 );
  T_quiet_rsc_success( status );

  return state;
}

static void event_with_timeout_from_isr_action( void *arg )
{
  rtems_status_code status;
  rtems_event_set   out;

  (void) arg;

  status = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 1, &out );
  T_quiet_true( status == RTEMS_SUCCESSFUL || status == RTEMS_TIMEOUT );
}

static const T_interrupt_test_config event_with_timeout_from_isr_config = {
  .action = event_with_timeout_from_isr_action,
  .interrupt = event_with_timeout_from_isr_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE( EventWithTimeoutFromISR )
{
  test_context           ctx;
  T_interrupt_test_state state;

  ctx.main_task = rtems_task_self();
  ctx.main_thread = _Thread_Get_executing();
  ctx.other_task = 0xdeadbeef;

  state = T_interrupt_test( &event_with_timeout_from_isr_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  clear_pending_events();
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MICROSECONDS_PER_TICK   1000

#define CONFIGURE_MAXIMUM_TASKS             2

#include <rtems/confdefs.h>
