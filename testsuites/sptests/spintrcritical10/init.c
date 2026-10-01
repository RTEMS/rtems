/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2013, 2026 embedded brains GmbH & Co. KG
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

#include <tmacros.h>
#include <rtems/test.h>

#include <rtems/score/threadimpl.h>
#include <rtems/rtems/eventimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 10";

#define MAX_ITERATION_COUNT 10000

#define GREEN RTEMS_EVENT_0

#define RED RTEMS_EVENT_1

#define EVENTS ( GREEN | RED )

#define DEADBEEF 0xdeadbeef

typedef struct {
  Thread_Control *thread;
} test_context;

static bool blocks_for_event( Thread_Wait_flags flags )
{
  return flags == THREAD_WAIT_CLASS_EVENT ||
         flags == ( THREAD_WAIT_CLASS_EVENT | THREAD_WAIT_STATE_BLOCKED );
}

static bool interrupts_blocking_op( Thread_Wait_flags flags )
{
  return flags == THREAD_WAIT_CLASS_EVENT;
}

static T_interrupt_test_state any_satisfy_before_timeout_interrupt( void *arg )
{
  rtems_status_code      sc;
  test_context          *ctx = arg;
  Thread_Control        *thread = ctx->thread;
  Thread_Wait_flags      wait_flags = _Thread_Wait_flags_get( thread );
  Thread_Wait_flags      flags = wait_flags & THREAD_WAIT_MASK;
  T_interrupt_test_state state;

  if ( blocks_for_event( flags ) ) {
    if ( interrupts_blocking_op( flags ) ) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_LATE;
    }

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    sc = rtems_event_send( thread->Object.id, GREEN );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    sc = rtems_event_send( thread->Object.id, RED );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    _Thread_Timeout( &thread->Timer.Watchdog, wait_flags );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == GREEN
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    if ( state == T_INTERRUPT_TEST_DONE ) {
      rtems_test_assert(
        ( _Thread_Wait_flags_get( thread ) & THREAD_WAIT_MASK ) ==
        THREAD_WAIT_STATE_READY
      );
    }

    rtems_test_assert( thread->Wait.count == EVENTS );
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  return state;
}

static void any_satisfy_before_timeout_action( void *arg )
{
  rtems_status_code sc;
  rtems_event_set   out;

  (void) arg;

  out = DEADBEEF;
  sc = rtems_event_receive( EVENTS, RTEMS_EVENT_ANY | RTEMS_WAIT, 1, &out );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == GREEN );

  out = DEADBEEF;
  sc = rtems_event_receive( EVENTS, RTEMS_EVENT_ANY | RTEMS_NO_WAIT, 0, &out );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == RED );
}

static const T_interrupt_test_config any_satisfy_before_timeout_config = {
  .action = any_satisfy_before_timeout_action,
  .interrupt = any_satisfy_before_timeout_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE( EventAnySatisfyBeforeTimeout )
{
  test_context           ctx;
  T_interrupt_test_state state;

  ctx.thread = _Thread_Get_executing();
  state = T_interrupt_test( &any_satisfy_before_timeout_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );
}

static T_interrupt_test_state all_satisfy_before_timeout_interrupt( void *arg )
{
  rtems_status_code      sc;
  test_context          *ctx = arg;
  Thread_Control        *thread = ctx->thread;
  Thread_Wait_flags      wait_flags = _Thread_Wait_flags_get( thread );
  Thread_Wait_flags      flags = wait_flags & THREAD_WAIT_MASK;
  T_interrupt_test_state state;

  if ( blocks_for_event( flags ) ) {
    if ( interrupts_blocking_op( flags ) ) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_LATE;
    }

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    sc = rtems_event_send( thread->Object.id, GREEN );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    sc = rtems_event_send( thread->Object.id, RED );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    _Thread_Timeout( &thread->Timer.Watchdog, wait_flags );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == EVENTS
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    if ( state == T_INTERRUPT_TEST_DONE ) {
      rtems_test_assert(
        ( _Thread_Wait_flags_get( thread ) & THREAD_WAIT_MASK ) ==
        THREAD_WAIT_STATE_READY
      );
    }

    rtems_test_assert( thread->Wait.count == EVENTS );
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  return state;
}

static void all_satisfy_before_timeout_action( void *arg )
{
  (void) arg;

  rtems_status_code sc;
  rtems_event_set   out;

  out = DEADBEEF;
  sc = rtems_event_receive( EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == EVENTS );
}

static const T_interrupt_test_config all_satisfy_before_timeout_config = {
  .action = all_satisfy_before_timeout_action,
  .interrupt = all_satisfy_before_timeout_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE( EventAllSatisfyBeforeTimeout )
{
  test_context           ctx;
  T_interrupt_test_state state;

  ctx.thread = _Thread_Get_executing();
  state = T_interrupt_test( &all_satisfy_before_timeout_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );
}

static T_interrupt_test_state timeout_before_satisfied_interrupt( void *arg )
{
  rtems_status_code      sc;
  test_context          *ctx = arg;
  Thread_Control        *thread = ctx->thread;
  Thread_Wait_flags      wait_flags = _Thread_Wait_flags_get( thread );
  Thread_Wait_flags      flags = wait_flags & THREAD_WAIT_MASK;
  T_interrupt_test_state state;

  if ( blocks_for_event( flags ) ) {
    if ( interrupts_blocking_op( flags ) ) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_LATE;
    }

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert(
      _Thread_Wait_get_status( thread ) == STATUS_SUCCESSFUL
    );

    _Thread_Timeout( &thread->Timer.Watchdog, wait_flags );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert( _Thread_Wait_get_status( thread ) == STATUS_TIMEOUT );

    sc = rtems_event_send( thread->Object.id, EVENTS );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    rtems_test_assert(
      *(rtems_event_set *) thread->Wait.return_argument == DEADBEEF
    );
    rtems_test_assert( _Thread_Wait_get_status( thread ) == STATUS_TIMEOUT );

    if ( state == T_INTERRUPT_TEST_DONE ) {
      rtems_test_assert(
        ( _Thread_Wait_flags_get( thread ) & THREAD_WAIT_MASK ) ==
        THREAD_WAIT_STATE_READY
      );
    }

    rtems_test_assert( thread->Wait.count == EVENTS );
  } else {
    state = T_INTERRUPT_TEST_EARLY;
  }

  return state;
}

static void timeout_before_satisfied_action( void *arg )
{
  (void) arg;

  rtems_event_set   out;
  rtems_status_code sc;

  out = DEADBEEF;
  sc = rtems_event_receive( EVENTS, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out );
  rtems_test_assert( sc == RTEMS_TIMEOUT );
  rtems_test_assert( out == DEADBEEF );

  out = DEADBEEF;
  sc = rtems_event_receive( EVENTS, RTEMS_EVENT_ALL | RTEMS_NO_WAIT, 0, &out );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == EVENTS );
}

static const T_interrupt_test_config timeout_before_satisfied_config = {
  .action = timeout_before_satisfied_action,
  .interrupt = timeout_before_satisfied_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE( EventTimeoutBeforeSatisfied )
{
  test_context           ctx;
  T_interrupt_test_state state;

  ctx.thread = _Thread_Get_executing();
  state = T_interrupt_test( &timeout_before_satisfied_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );
}

static rtems_id message_queue;

static bool message_receive_done;

static T_interrupt_test_state message_receive_interrupt( void *arg )
{
  rtems_status_code      sc;
  test_context          *ctx = arg;
  Thread_Control        *thread = ctx->thread;
  Thread_Wait_flags      wait_flags = _Thread_Wait_flags_get( thread );
  Thread_Wait_flags      flags = wait_flags & THREAD_WAIT_MASK;
  T_interrupt_test_state state;
  long                   buf;
  size_t                 size;

  if ( !interrupts_blocking_op( flags ) ) {
    return blocks_for_event( flags ) ? T_INTERRUPT_TEST_LATE
                                     : T_INTERRUPT_TEST_EARLY;
  }

  state = T_INTERRUPT_TEST_DONE;
  rtems_test_assert( thread->Wait.count == GREEN );

  sc = rtems_message_queue_receive(
    message_queue,
    &buf,
    &size,
    RTEMS_NO_WAIT,
    0
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  message_receive_done = true;

  sc = rtems_event_send( thread->Object.id, GREEN );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  return state;
}

static void message_receive_action( void *arg )
{
  rtems_status_code sc;
  rtems_event_set   out;
  long              buf;
  uint32_t          count;

  (void) arg;

  buf = 0;
  (void) rtems_message_queue_flush( message_queue, &count );
  sc = rtems_message_queue_send( message_queue, &buf, sizeof( buf ) );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  message_receive_done = false;
  out = DEADBEEF;
  sc = rtems_event_receive( GREEN, RTEMS_EVENT_ALL | RTEMS_WAIT, 1, &out );

  if ( message_receive_done ) {
    T_rsc_success( sc );
    T_eq_u32( out, GREEN );
  }
}

static const T_interrupt_test_config message_receive_config = {
  .action = message_receive_action,
  .interrupt = message_receive_interrupt,
  .max_iteration_count = MAX_ITERATION_COUNT
};

T_TEST_CASE( EventMessageReceiveInInterrupt )
{
  rtems_status_code      sc;
  test_context           ctx;
  T_interrupt_test_state state;

  sc = rtems_message_queue_create(
    rtems_build_name( 'M', 'S', 'G', 'Q' ),
    1,
    sizeof( long ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &message_queue
  );
  T_rsc_success( sc );

  ctx.thread = _Thread_Get_executing();
  state = T_interrupt_test( &message_receive_config, &ctx );
  T_eq_int( state, T_INTERRUPT_TEST_DONE );

  sc = rtems_message_queue_delete( message_queue );
  T_rsc_success( sc );
}

static rtems_task Init( rtems_task_argument argument )
{
  (void) argument;

  rtems_test_run( argument, TEST_STATE );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( 1, sizeof( long ) )
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
