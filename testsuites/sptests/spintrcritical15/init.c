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

const char rtems_test_name[] = "SPINTRCRITICAL 15";

#define INIT_PRIORITY      2
#define BLOCKER_PRIORITY   1

typedef struct {
  rtems_id secondary_task_id;
  rtems_id semaphore;
  Thread_Control *main_thread;
} test_context;

static rtems_task Secondary_task( rtems_task_argument arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = (test_context *) arg;

  while (1) {
    sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
    T_quiet_rsc( sc, RTEMS_TIMEOUT );
  }
}

static T_interrupt_test_state interrupt(void *arg)
{
  test_context           *ctx;
  Thread_Wait_flags       flags;
  T_interrupt_test_state  state;

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->main_thread );

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

  return state;
}

static void prepare( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;
  sc = rtems_task_restart( ctx->secondary_task_id, (rtems_task_argument) ctx );
  T_quiet_rsc_success( sc );
}

static void action( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;
  sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
  T_quiet_rsc( sc, RTEMS_TIMEOUT );
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE( SemaphoreObtainBlockedInterrupt )
{
  test_context           ctx;
  rtems_status_code      sc;
  T_interrupt_test_state state;

  ctx.main_thread = _Thread_Get_executing();

  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &ctx.semaphore
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_create(
    rtems_build_name( 'B', 'L', 'C', 'K' ),
    BLOCKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx.secondary_task_id
  );
  T_assert_rsc_success( sc );

  sc = rtems_task_start(
    ctx.secondary_task_id,
    Secondary_task,
    (rtems_task_argument) &ctx
  );
  T_assert_rsc_success( sc );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_task_delete( ctx.secondary_task_id );
  T_rsc_success( sc );

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

#define CONFIGURE_MAXIMUM_TASKS          2
#define CONFIGURE_MAXIMUM_SEMAPHORES     1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INIT_TASK_PRIORITY  INIT_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
