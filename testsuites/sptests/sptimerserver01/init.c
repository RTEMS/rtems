/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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
