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

#include <pthread.h>
#include <signal.h>

#define TEST_SIGNAL SIGUSR1

typedef enum {
  SIG_READY,
  SIG_SENT,
  SIG_PROCESSED
} test_state;

typedef struct {
  test_state state;
  pthread_t consumer;
  pthread_t producer;
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
  .state = SIG_READY
};

static void signal_handler(int signum)
{
  test_context *ctx = &ctx_instance;

  switch (ctx->state) {
    case SIG_SENT:
      change_state(ctx, SIG_PROCESSED);
      break;
    default:
      rtems_test_assert(0);
  }
}

static void signal_send(test_context *ctx, test_state new_state)
{
  int eno;

  eno = pthread_kill(ctx->consumer, TEST_SIGNAL);
  rtems_test_assert(eno == 0);

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

static void *producer(void *arg)
{
  test_context *ctx = arg;

  ctx->producer_processor = rtems_smp_get_current_processor();

  rtems_test_assert(ctx->consumer_processor != ctx->producer_processor);

  wait_for_state(ctx, SIG_READY);
  signal_send(ctx, SIG_SENT);

  check_producer_processor(ctx);

  return ctx;
}

static void test(void)
{
  test_context *ctx = &ctx_instance;
  struct sigaction new_action;
  sigset_t test_signal_set;
  int rv;
  pthread_attr_t attr;
  int eno;
  void *producer_status;

  ctx->consumer = pthread_self();
  ctx->consumer_processor = rtems_smp_get_current_processor();

  memset(&new_action, 0, sizeof(new_action));
  new_action.sa_handler = signal_handler;

  rv = sigaction(TEST_SIGNAL, &new_action, NULL);
  rtems_test_assert(rv == 0);

  rv = sigemptyset(&test_signal_set);
  rtems_test_assert(rv == 0);

  rv = sigaddset(&test_signal_set, TEST_SIGNAL);
  rtems_test_assert(rv == 0);

  eno = pthread_sigmask(SIG_UNBLOCK, &test_signal_set, NULL);
  rtems_test_assert(eno == 0);

  eno = pthread_attr_init(&attr);
  rtems_test_assert(eno == 0);

  eno = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  rtems_test_assert(eno == 0);

  eno = pthread_create(&ctx->producer, &attr, producer, ctx);
  rtems_test_assert(eno == 0);

  eno = pthread_attr_destroy(&attr);
  rtems_test_assert(eno == 0);

  check_consumer_processor(ctx);

  wait_for_state(ctx, SIG_PROCESSED);

  check_consumer_processor(ctx);

  producer_status = NULL;
  pthread_join(ctx->producer, &producer_status);
  rtems_test_assert(eno == 0);
  rtems_test_assert(producer_status == ctx);
}

static void *POSIX_Init(void *arg)
{
  puts("\n\n*** TEST SMPPSXSIGNAL 1 ***");

  if (rtems_smp_get_processor_count() >= 2) {
    test();
  }

  puts("*** END OF TEST SMPPSXSIGNAL 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
