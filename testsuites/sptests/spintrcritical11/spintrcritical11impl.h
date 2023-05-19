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

#if defined(EVENT_ANY)
  #define TEST_NAME          "11"
  #define TEST_STRING        Any
  #define EVENTS_TO_SEND     0x1
  #define EVENTS_TO_RECEIVE  0x3

#elif defined(EVENT_ALL)
  #define TEST_NAME          "12"
  #define TEST_STRING        All
  #define EVENTS_TO_SEND     0x3
  #define EVENTS_TO_RECEIVE  0x3

#else
  #error "Test Mode not defined"

#endif

const char rtems_test_name[] = "SPINTRCRITICAL " TEST_NAME;

typedef struct {
  rtems_id      main_task;
  long          potential_hits;
  volatile bool early;
  volatile bool late;
} test_context;

static T_interrupt_test_state interrupt( void *arg )
{
  test_context           *ctx;
  T_interrupt_test_state  state;
  rtems_status_code       sc;

  state = T_interrupt_test_get_state();

  if ( state != T_INTERRUPT_TEST_ACTION ) {
    return T_INTERRUPT_TEST_CONTINUE;
  }

  ctx = arg;
  sc = rtems_event_send( ctx->main_task, EVENTS_TO_SEND );
  T_quiet_rsc_success( sc );

  if ( ctx->early ) {
    state = T_INTERRUPT_TEST_EARLY;
  } else if ( ctx->late ) {
    state = T_INTERRUPT_TEST_LATE;
  } else {
    ++ctx->potential_hits;
    state = T_INTERRUPT_TEST_CONTINUE;
  }

  return state;
}

static void prepare( void *arg )
{
  test_context    *ctx;
  rtems_event_set  out;

  ctx = arg;
  ctx->early = true;
  ctx->late = false;
  (void ) rtems_event_receive( RTEMS_PENDING_EVENTS, RTEMS_NO_WAIT, 0, &out );
}

static void action( void *arg )
{
  test_context    *ctx;
  rtems_event_set  out;

  ctx = arg;
  ctx->early = false;
  (void ) rtems_event_receive( EVENTS_TO_RECEIVE, RTEMS_EVENT_ANY, 1, &out );
  ctx->late = true;
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 1000
};

T_TEST_CASE( RTEMS_XCONCAT( EventReceiveInterrupt, TEST_STRING ) )
{
  test_context           ctx;
  T_interrupt_test_state state;

  memset( &ctx, 0, sizeof( ctx ) );
  ctx.main_task = rtems_task_self();

  state = T_interrupt_test( &config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_TIMEOUT );

  T_log( T_NORMAL, "potential hits = %ld", ctx.potential_hits );
  T_gt_long( ctx.potential_hits, 0 );
}

static rtems_task Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
