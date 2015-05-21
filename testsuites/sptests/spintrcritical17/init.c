/*
 * Copyright (c) 2009-2014 embedded brains GmbH.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#include <rtems/rtems/timerimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 17";

typedef struct {
  rtems_id timer1;
  rtems_id timer2;
  bool done;
} test_context;

static test_context ctx_instance;

static void never(rtems_id timer_id, void *arg)
{
  rtems_test_assert(0);
}

static void fire(rtems_id timer_id, void *arg)
{
  /* The arg is NULL */
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  if (!ctx->done) {
    ctx->done =
      _Timer_server->Interval_watchdogs.system_watchdog_helper != NULL;

    if (ctx->done) {
      sc = rtems_timer_server_fire_after(ctx->timer2, 100, never, NULL);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }
}

static bool test_body(void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_timer_reset(ctx->timer1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return ctx->done;
}

static void Init(rtems_task_argument ignored)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;

  TEST_BEGIN();

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', '1'),
    &ctx->timer1
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_create(
    rtems_build_name('T', 'I', 'M', '2'),
    &ctx->timer2
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_server_fire_after(ctx->timer1, 1000, never, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  interrupt_critical_section_test(test_body, ctx, fire);
  rtems_test_assert(ctx->done);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 3
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
