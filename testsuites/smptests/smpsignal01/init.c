/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2017 embedded brains GmbH & Co. KG
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

const char rtems_test_name[] = "SMPSIGNAL 1";

#define TEST_SIGNAL RTEMS_SIGNAL_0

typedef enum {
  SIG_0_READY,
  SIG_0_SENT,
  SIG_0_ENABLE,
  SIG_0_PROCESSED,
  SIG_1_READY,
  SIG_1_SENT,
  SIG_1_PROCESSED
} test_state;

typedef struct {
  volatile test_state state;
  rtems_id consumer;
  rtems_id producer;
  uint32_t consumer_processor;
  uint32_t producer_processor;
  rtems_id timer;
  volatile bool done;
} test_context;

static void change_state(test_context *ctx, test_state new_state)
{
  ctx->state = new_state;
}

static void wait_for_state(const test_context *ctx, test_state desired_state)
{
  while ( ctx->state != desired_state ) {
    /* Wait */
  }
}

static test_context ctx_instance = {
  .state = SIG_0_READY
};

static void signal_handler(rtems_signal_set signal)
{
  test_context *ctx = &ctx_instance;

  rtems_test_assert(_ISR_Get_level() == 0);

  switch (ctx->state) {
    case SIG_0_ENABLE:
      change_state(ctx, SIG_0_PROCESSED);
      break;
    case SIG_1_SENT:
      change_state(ctx, SIG_1_PROCESSED);
      break;
    default:
      rtems_test_assert(0);
  }
}

static void signal_send(test_context *ctx, test_state new_state)
{
  rtems_status_code sc;

  change_state(ctx, new_state);
  sc = rtems_signal_send(ctx->consumer, TEST_SIGNAL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

}

static void check_consumer_processor(const test_context *ctx)
{
  rtems_test_assert(
    ctx->consumer_processor == rtems_scheduler_get_processor()
  );
}

static void check_producer_processor(const test_context *ctx)
{
  rtems_test_assert(
    ctx->producer_processor == rtems_scheduler_get_processor()
  );
}

static void producer(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  ctx->producer_processor = rtems_scheduler_get_processor();

  rtems_test_assert(ctx->consumer_processor != ctx->producer_processor);

  wait_for_state(ctx, SIG_0_READY);
  signal_send(ctx, SIG_0_SENT);

  check_producer_processor(ctx);

  wait_for_state(ctx, SIG_1_READY);
  signal_send(ctx, SIG_1_SENT);

  check_producer_processor(ctx);

  rtems_task_suspend(RTEMS_SELF);
  rtems_test_assert(0);
}

static void test_two_processors(test_context *ctx)
{
  rtems_status_code sc;
  rtems_mode mode;

  sc = rtems_signal_catch(signal_handler, RTEMS_DEFAULT_MODES);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('P', 'R', 'O', 'D'),
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->producer
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->producer, producer, (rtems_task_argument) ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  check_consumer_processor(ctx);

  wait_for_state(ctx, SIG_0_SENT);
  change_state(ctx, SIG_0_ENABLE);

  sc = rtems_task_mode(RTEMS_ASR, RTEMS_ASR_MASK, &mode);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(mode == RTEMS_NO_ASR);

  wait_for_state(ctx, SIG_0_PROCESSED);

  check_consumer_processor(ctx);

  change_state(ctx, SIG_1_READY);
  wait_for_state(ctx, SIG_1_PROCESSED);

  check_consumer_processor(ctx);
}

static void isr_level_timer(rtems_id timer, void *arg)
{
  test_context *ctx = arg;
  rtems_status_code sc;

  sc = rtems_signal_send(ctx->consumer, TEST_SIGNAL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void isr_level_handler(rtems_signal_set signal)
{
  test_context *ctx = &ctx_instance;

  rtems_test_assert(_ISR_Get_level() == 0);

  ctx->done = true;
}

static void test_isr_level(test_context *ctx)
{
  rtems_status_code sc;
  rtems_mode mode;

  sc = rtems_task_mode(RTEMS_ASR, RTEMS_ASR_MASK, &mode);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(mode == RTEMS_NO_ASR);

  sc = rtems_timer_create(rtems_build_name('T', 'I', 'M', 'R'), &ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rtems_test_assert(!ctx->done);

  sc = rtems_signal_catch(isr_level_handler, RTEMS_DEFAULT_MODES);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(ctx->timer, 1, isr_level_timer, ctx);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!ctx->done) {
    /* Wait for timer */
  }

  sc = rtems_timer_delete(ctx->timer);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_mode(RTEMS_NO_ASR, RTEMS_ASR_MASK, &mode);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(mode == RTEMS_ASR);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &ctx_instance;

  TEST_BEGIN();

  ctx->consumer = rtems_task_self();
  ctx->consumer_processor = rtems_scheduler_get_processor();

  test_isr_level(ctx);

  if (rtems_scheduler_get_processor_maximum() >= 2) {
    test_two_processors(ctx);
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_ASR

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
