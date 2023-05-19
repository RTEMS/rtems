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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems/score/threadimpl.h>

#if defined(FIFO_NO_TIMEOUT)
  #define TEST_NAME                "1"
  #define TEST_STRING              FIFOWithoutTimeout
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_DEFAULT_ATTRIBUTES

#elif defined(FIFO_WITH_TIMEOUT)
  #define TEST_NAME                "2"
  #define TEST_STRING              FIFOWithTimeout
  #define SEMAPHORE_OBTAIN_TIMEOUT 10
  #define SEMAPHORE_ATTRIBUTES     RTEMS_DEFAULT_ATTRIBUTES

#elif defined(PRIORITY_NO_TIMEOUT)
  #define TEST_NAME                "3"
  #define TEST_STRING              PriorityWithoutTimeout
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#elif defined(PRIORITY_WITH_TIMEOUT)
  #define TEST_NAME                "4"
  #define TEST_STRING              PriorityWithTimeout
  #define SEMAPHORE_OBTAIN_TIMEOUT 10
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#elif defined(PRIORITY_NO_TIMEOUT_REVERSE)
  #define TEST_NAME                "5"
  #define TEST_STRING              PriorityWithoutTimeoutReverse
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#else

  #error "Test Mode not defined"
#endif

const char rtems_test_name[] = "SPINTRCRITICAL " TEST_NAME;

typedef struct {
  Thread_Control *thread;
  rtems_id        semaphore;
} test_context;

static bool is_blocked( Thread_Wait_flags flags )
{
  return flags == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED );
}

static bool interrupts_blocking_op( Thread_Wait_flags flags )
{
  return flags
    == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK );
}

static T_interrupt_test_state interrupt( void *arg )
{
  test_context           *ctx;
  T_interrupt_test_state  state;
  Thread_Wait_flags       flags;
  rtems_status_code       status;

  ctx = arg;
  flags = _Thread_Wait_flags_get( ctx->thread );

  if ( interrupts_blocking_op( flags ) ) {
    state = T_INTERRUPT_TEST_DONE;
  } else if ( is_blocked( flags ) ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  status = rtems_semaphore_release( ctx->semaphore );
  T_quiet_rsc_success( status );

  return state;
}

static void action( void *arg )
{
  test_context      *ctx;
  rtems_status_code  status;

  ctx = arg;
  status = rtems_semaphore_obtain(
    ctx->semaphore,
    RTEMS_DEFAULT_OPTIONS,
    SEMAPHORE_OBTAIN_TIMEOUT
  );
  T_quiet_rsc_success( status );
}

static const T_interrupt_test_config config = {
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE( RTEMS_XCONCAT( SemaphoreRelease, TEST_STRING ) )
{
  test_context           ctx;
  rtems_status_code      status;
  T_interrupt_test_state state;

  ctx.thread = _Thread_Get_executing();

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    SEMAPHORE_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &ctx.semaphore
  );
  T_rsc_success( status );

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  rtems_semaphore_delete( ctx.semaphore );
  T_rsc_success( status );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#if defined(PRIORITY_NO_TIMEOUT_REVERSE)
  #define CONFIGURE_INIT_TASK_PRIORITY   250
#endif
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
