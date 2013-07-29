/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

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
  test_state state;
  rtems_id consumer;
  rtems_id producer;
  uint32_t consumer_processor;
  uint32_t producer_processor;
} test_context;

static void change_state(test_context *ctx, test_state new_state)
{
  ctx->state = new_state;
  _CPU_SMP_Processor_event_broadcast();
}

static void wait_for_state(const test_context *ctx, test_state desired_state)
{
  while ( ctx->state != desired_state ) {
    _CPU_SMP_Processor_event_receive();
  }
}

static test_context ctx_instance = {
  .state = SIG_0_READY
};

static void signal_handler(rtems_signal_set signal)
{
  test_context *ctx = &ctx_instance;

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

  sc = rtems_signal_send(ctx->consumer, TEST_SIGNAL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  change_state(ctx, new_state);
}

static void check_consumer_processor(const test_context *ctx)
{
  rtems_test_assert(
    ctx->consumer_processor == rtems_smp_get_current_processor()
  );
}

static void check_producer_processor(const test_context *ctx)
{
  rtems_test_assert(
    ctx->producer_processor == rtems_smp_get_current_processor()
  );
}

static void producer(rtems_task_argument arg)
{
  test_context *ctx = (test_context *) arg;

  ctx->producer_processor = rtems_smp_get_current_processor();

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

static void test(void)
{
  test_context *ctx = &ctx_instance;
  rtems_status_code sc;
  rtems_mode mode;

  ctx->consumer = rtems_task_self();
  ctx->consumer_processor = rtems_smp_get_current_processor();

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

  wait_for_state(ctx, SIG_0_PROCESSED);

  check_consumer_processor(ctx);

  change_state(ctx, SIG_1_READY);
  wait_for_state(ctx, SIG_1_PROCESSED);

  check_consumer_processor(ctx);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SMPSIGNAL 1 ***");

  if (rtems_smp_get_processor_count() >= 2) {
    test();
  }

  puts("*** END OF TEST SMPSIGNAL 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_ASR

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
