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
#include <time.h>

#include <rtems/test.h>
#include <rtems/test-info.h>

const char rtems_test_name[] = "PSXINTRCRITICAL 1";

typedef struct {
  timer_t           timer;
  struct itimerspec spec;
  volatile bool early;
  volatile bool late;
  long early_count;
  long late_count;
  long potential_hits;
} test_context;

#define POSIX_TIMER_RELATIVE 0

static void prepare(void *arg)
{
  test_context *ctx;

  ctx = arg;
  ctx->early = false;
  ctx->late = false;
}

static void action(void *arg)
{
  test_context *ctx;
  int rv;

  ctx = arg;
  ctx->early = true;
  rv = timer_settime(ctx->timer, POSIX_TIMER_RELATIVE, &ctx->spec, NULL);
  ctx->late = true;
  T_quiet_psx_success(rv);

  T_interrupt_test_busy_wait_for_interrupt();
}

static T_interrupt_test_state interrupt(void *arg)
{
  test_context *ctx;
  int rv;

  if (T_interrupt_test_get_state() != T_INTERRUPT_TEST_ACTION) {
    return T_INTERRUPT_TEST_EARLY;
  }

  ctx = arg;
  rv = timer_settime(ctx->timer, POSIX_TIMER_RELATIVE, &ctx->spec, NULL);
  T_quiet_psx_success(rv);

  if (ctx->late) {
    ++ctx->late_count;
    return T_INTERRUPT_TEST_LATE;
  } else if (ctx->early) {
    ++ctx->early_count;
    return T_INTERRUPT_TEST_EARLY;
  } else {
    ++ctx->potential_hits;

    if (ctx->potential_hits > 13) {
      return T_INTERRUPT_TEST_DONE;
    } else {
      return T_INTERRUPT_TEST_CONTINUE;
    }
  }
}

static const T_interrupt_test_config config = {
  .prepare = prepare,
  .action = action,
  .interrupt = interrupt,
  .max_iteration_count = 10000
};

T_TEST_CASE(PSXSetTimerInterrupt)
{
  test_context ctx;
  int sc;
  T_interrupt_test_state state;

  memset(&ctx, 0, sizeof(ctx));

  /* create POSIX Timer */
  sc = timer_create (CLOCK_REALTIME, NULL, &ctx.timer);
  T_psx_success(sc);

  /* we don't care if it ever fires */
  ctx.spec.it_interval.tv_sec  = 10;
  ctx.spec.it_value.tv_sec     = 10;

  state = T_interrupt_test(&config, &ctx);
  T_eq_int(state, T_INTERRUPT_TEST_DONE);

  T_log(T_NORMAL, "early count = %ld", ctx.early_count);
  T_log(T_NORMAL, "late count = %ld", ctx.late_count);
  T_log(T_NORMAL, "potential hits = %ld", ctx.potential_hits);
  T_gt_int(ctx.potential_hits, 0);

  sc = timer_delete(ctx.timer);
  T_psx_success(sc);
}

static rtems_task Init(rtems_task_argument arg)
{
  rtems_test_run(arg, TEST_STATE);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_POSIX_TIMERS   1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
