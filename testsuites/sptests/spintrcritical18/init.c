/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

const char rtems_test_name[] = "SPINTRCRITICAL 18";

#define WAKE_UP RTEMS_EVENT_0

#define PRIORITY_LOW 3

#define PRIORITY_MIDDLE 2

#define PRIORITY_HIGH 1

#define ASSERT_SC(sc) rtems_test_assert( (sc) == RTEMS_SUCCESSFUL )

typedef struct {
  rtems_id middle_priority_task;
  rtems_id high_priority_task;
  bool high_priority_task_activated;
} test_context;

static test_context global_ctx;

static void wake_up(rtems_id task)
{
  rtems_status_code sc;

  sc = rtems_event_send( task, WAKE_UP );
  ASSERT_SC( sc );
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
  ASSERT_SC( sc );
  rtems_test_assert( events == WAKE_UP );
}

static void active_high_priority_task( rtems_id timer, void *arg )
{
  /* The arg is NULL */
  test_context *ctx = &global_ctx;

  rtems_test_assert( !ctx->high_priority_task_activated );
  ctx->high_priority_task_activated = true;
  wake_up( ctx->high_priority_task );
}

static void middle_priority_task( rtems_task_argument arg )
{
  test_context *ctx = (test_context *) arg;

  while ( true ) {
    wait_for_wake_up();

    rtems_test_assert( !ctx->high_priority_task_activated );
  }
}

static void high_priority_task( rtems_task_argument arg )
{
  test_context *ctx = (test_context *) arg;

  while ( true ) {
    wait_for_wake_up();

    rtems_test_assert( ctx->high_priority_task_activated );
    ctx->high_priority_task_activated = false;
  }
}

static bool test_body( void *arg )
{
  test_context *ctx = arg;

  wake_up( ctx->middle_priority_task );

  return false;
}

static void Init( rtems_task_argument ignored )
{
  test_context *ctx = &global_ctx;
  rtems_status_code sc;

  TEST_BEGIN();

  sc = rtems_task_create(
    rtems_build_name( 'H', 'I', 'G', 'H' ),
    PRIORITY_HIGH,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high_priority_task
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(
    ctx->high_priority_task,
    high_priority_task,
    (rtems_task_argument) ctx
  );
  ASSERT_SC(sc);

  sc = rtems_task_create(
    rtems_build_name( 'M', 'I', 'D', 'L' ),
    PRIORITY_MIDDLE,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->middle_priority_task
  );
  ASSERT_SC(sc);

  sc = rtems_task_start(
    ctx->middle_priority_task,
    middle_priority_task,
    (rtems_task_argument) ctx
  );
  ASSERT_SC(sc);

  interrupt_critical_section_test( test_body, ctx, active_high_priority_task );

  TEST_END();

  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 3
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INIT_TASK_PRIORITY PRIORITY_LOW
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
