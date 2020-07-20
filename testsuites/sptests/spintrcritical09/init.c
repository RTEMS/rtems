/*
 *  Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <rtems/score/threadimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 9";

typedef struct {
  Thread_Control *thread;
  rtems_id        semaphore;
  volatile bool   early;
} test_context;

static bool is_interrupt_timeout( test_context *ctx )
{
  Thread_Wait_flags flags = _Thread_Wait_flags_get( ctx->thread );

  return flags == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_READY_AGAIN );
}

static T_interrupt_test_state interrupt( void *arg )
{
  test_context           *ctx;
  Per_CPU_Control        *cpu_self;
  Watchdog_Header        *header;
  Watchdog_Control       *watchdog;
  T_interrupt_test_state  state;

  ctx = arg;
  cpu_self = _Per_CPU_Get();
  header = &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  watchdog = (Watchdog_Control *) header->first;

  if (
    watchdog != NULL
      && watchdog->expire == cpu_self->Watchdog.ticks
      && watchdog->routine == _Thread_Timeout
  ) {
    ISR_Level level;

    _ISR_Local_disable( level );
    _Watchdog_Per_CPU_remove( watchdog, cpu_self, header );
    _ISR_Local_enable( level );

    ( *watchdog->routine )( watchdog );

    if ( is_interrupt_timeout( ctx ) ) {
      state = T_INTERRUPT_TEST_DONE;
    } else {
      state = T_INTERRUPT_TEST_LATE;
    }
  } else {
    if ( ctx->early ) {
      state = T_INTERRUPT_TEST_EARLY;
    } else {
      state = T_INTERRUPT_TEST_LATE;
    }
  }

  return state;
}

static void prepare( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;
  ctx->early = true;
  sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_NO_WAIT, 0 );
  T_quiet_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED );
}

static void action( void *arg )
{
  test_context      *ctx;
  rtems_status_code  sc;

  ctx = arg;
  ctx->early = false;
  sc = rtems_semaphore_obtain( ctx->semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
  T_quiet_rsc( sc, RTEMS_TIMEOUT );
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE( SemaphoreObtainInterrupt )
{
  test_context           ctx;
  rtems_status_code      sc;
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

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
