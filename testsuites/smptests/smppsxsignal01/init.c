/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#include <pthread.h>
#include <signal.h>

const char rtems_test_name[] = "SMPPSXSIGNAL 1";

#define TEST_SIGNAL SIGUSR1

typedef enum {
  SIG_READY,
  SIG_SENT,
  SIG_PROCESSED
} test_state;

typedef struct {
  volatile test_state state;
  pthread_t consumer;
  pthread_t producer;
  uint32_t consumer_processor;
  uint32_t producer_processor;
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
    ctx->consumer_processor == rtems_scheduler_get_processor()
  );
}

static void check_producer_processor(const test_context *ctx)
{
  rtems_test_assert(
    ctx->producer_processor == rtems_scheduler_get_processor()
  );
}

static void *producer(void *arg)
{
  test_context *ctx = arg;

  ctx->producer_processor = rtems_scheduler_get_processor();

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
  ctx->consumer_processor = rtems_scheduler_get_processor();

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
  TEST_BEGIN();

  if (rtems_scheduler_get_processor_maximum() >= 2) {
    test();
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
