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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems/score/threadimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 16";

typedef struct {
  Thread_Control *thread;
  rtems_id        semaphore;
} test_context;

static T_interrupt_test_state interrupt( void *arg )
{
  test_context *ctx;
  T_interrupt_test_state state;
  Thread_Wait_flags flags;
  rtems_status_code sc;

  state = T_interrupt_test_get_state();

  if (state != T_INTERRUPT_TEST_ACTION) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->thread );

  if (
    flags == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK )
  ) {
    state = T_INTERRUPT_TEST_DONE;
  } else if (
    flags == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED )
  ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  sc = rtems_semaphore_release( ctx->semaphore );
  T_quiet_rsc_success( sc );

  return state;
}

static void prepare( void *arg )
{
  test_context *ctx;
  rtems_status_code sc;

  ctx = arg;

  do {
    sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_NO_WAIT, 0 );
  } while ( sc == RTEMS_SUCCESSFUL );
}

static void action( void *arg )
{
  test_context *ctx;
  rtems_status_code sc;

  ctx = arg;
  sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_DEFAULT_OPTIONS, 2 );
  T_quiet_rsc_success( sc );
}

static void blocked( void *arg )
{
  test_context *ctx;
  rtems_status_code sc;

  T_interrupt_test_change_state(
    T_INTERRUPT_TEST_ACTION,
    T_INTERRUPT_TEST_LATE
  );

  ctx = arg;
  sc = rtems_semaphore_release( ctx->semaphore );
  T_quiet_rsc_success( sc );
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .blocked = blocked,
  .max_iteration_count = 10000
};

/*
 * Trying to generate timeout of a thread that had its blocking request
 * satisfied while blocking but before time timeout.
 */
T_TEST_CASE( SemaphoreSatisfyBeforeTimeout )
{
  test_context ctx;
  rtems_status_code sc;
  T_interrupt_test_state state;

  ctx.thread = _Thread_Get_executing();

  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &ctx.semaphore
  );
  T_assert_rsc_success( sc );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_semaphore_delete( ctx.semaphore );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_SEMAPHORES     1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
