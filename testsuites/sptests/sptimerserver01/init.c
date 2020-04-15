/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include "tmacros.h"

const char rtems_test_name[] = "SPTIMERSERVER 1";

#define TIMER_COUNT 2

typedef struct {
  rtems_id timer[TIMER_COUNT];
  rtems_id master;
} test_context;

static test_context ctx_instance;

static const rtems_time_of_day start = {
  .year = 2016,
  .month = 3,
  .day = 1,
  .hour = 12,
  .minute = 5,
  .second = 17
};

static void cancel(rtems_id id, void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_timer_cancel(ctx->timer[1]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_send(ctx->master);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void never(rtems_id id, void *arg)
{
  rtems_test_assert(0);
}

static void test(void)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;
  size_t i;
  rtems_time_of_day later;

  ctx->master = rtems_task_self();

  sc = rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < TIMER_COUNT; ++i) {
    sc = rtems_timer_create(
      rtems_build_name('T', 'M', 'R', '0' + i),
      &ctx->timer[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  sc = rtems_timer_server_fire_after(ctx->timer[0], 10, cancel, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_server_fire_after(ctx->timer[1], 10, never, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_clock_set(&start);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  later = start;
  ++later.second;

  sc = rtems_timer_server_fire_when(ctx->timer[0], &later, cancel, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_server_fire_when(ctx->timer[1], &later, never, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_event_transient_receive(RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS TIMER_COUNT

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
