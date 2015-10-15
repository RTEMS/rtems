/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>
#include <inttypes.h>

#include <rtems/test.h>

const char rtems_test_name[] = "TMFINE 1";

#define CPU_COUNT 32

#define MSG_COUNT 3

typedef struct {
  uint32_t value;
} test_msg;

typedef struct {
  rtems_test_parallel_context base;
  rtems_id master;
  rtems_id sema[CPU_COUNT];
  rtems_id mq[CPU_COUNT];
  uint32_t self_event_ops[CPU_COUNT][CPU_COUNT];
  uint32_t all_to_one_event_ops[CPU_COUNT][CPU_COUNT];
  uint32_t one_mutex_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_mutex_ops[CPU_COUNT][CPU_COUNT];
  uint32_t self_msg_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_to_one_msg_ops[CPU_COUNT][CPU_COUNT];
} test_context;

static test_context test_instance;

static rtems_interval test_duration(void)
{
  return rtems_clock_get_ticks_per_second();
}

static rtems_interval test_init(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  return test_duration();
}

static void test_fini(
  const char *name,
  uint32_t *counters,
  size_t active_workers
)
{
  size_t i;

  printf("  <%s activeWorker=\"%zu\">\n", name, active_workers);

  for (i = 0; i < active_workers; ++i) {
    printf(
      "    <Counter worker=\"%zu\">%" PRIu32 "</Counter>\n",
      i,
      counters[i]
    );
  }

  printf("  </%s>\n", name);
}

static void test_self_event_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = rtems_task_self();
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;
    rtems_event_set out;

    ++counter;

    sc = rtems_event_send(id, RTEMS_EVENT_0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_event_receive(
      RTEMS_EVENT_0,
      RTEMS_WAIT | RTEMS_EVENT_ANY,
      RTEMS_NO_TIMEOUT,
      &out
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->self_event_ops[active_workers - 1][worker_index] = counter;
}

static void test_self_event_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "SelfEvent",
    &ctx->self_event_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_all_to_one_event_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = rtems_task_self();
  bool is_master = rtems_test_parallel_is_master_worker(worker_index);
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_event_send(id, RTEMS_EVENT_0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    if (is_master) {
      rtems_event_set out;

      sc = rtems_event_receive(
        RTEMS_ALL_EVENTS,
        RTEMS_WAIT | RTEMS_EVENT_ANY,
        RTEMS_NO_TIMEOUT,
        &out
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }

  ctx->all_to_one_event_ops[active_workers - 1][worker_index] = counter;
}

static void test_all_to_one_event_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "AllToOneEvent",
    &ctx->all_to_one_event_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_one_mutex_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = ctx->sema[0];
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->one_mutex_ops[active_workers - 1][worker_index] = counter;
}

static void test_one_mutex_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "OneMutex",
    &ctx->one_mutex_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_mutex_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = ctx->sema[worker_index];
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->many_mutex_ops[active_workers - 1][worker_index] = counter;
}

static void test_many_mutex_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "ManyMutex",
    &ctx->many_mutex_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_self_msg_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = ctx->mq[worker_index];
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;
    test_msg msg = { .value = 0 };
    size_t n;

    ++counter;

    sc = rtems_message_queue_send(id, &msg, sizeof(msg));
    rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_TOO_MANY);

    n = sizeof(msg);
    sc = rtems_message_queue_receive(
      id,
      &msg,
      &n,
      RTEMS_WAIT,
      RTEMS_NO_TIMEOUT
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(n == sizeof(msg));
  }

  ctx->self_msg_ops[active_workers - 1][worker_index] = counter;
}

static void test_self_msg_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "SelfMsg",
    &ctx->self_msg_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_to_one_msg_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  test_context *ctx = (test_context *) base;
  rtems_id id = ctx->mq[0];
  bool is_master = rtems_test_parallel_is_master_worker(worker_index);
  uint32_t counter = 0;

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;
    test_msg msg = { .value = 0 };
    size_t n;

    ++counter;

    sc = rtems_message_queue_send(id, &msg, sizeof(msg));
    rtems_test_assert(sc == RTEMS_SUCCESSFUL || sc == RTEMS_TOO_MANY);

    if (is_master) {
      n = sizeof(msg);
      sc = rtems_message_queue_receive(
        id,
        &msg,
        &n,
        RTEMS_WAIT,
        RTEMS_NO_TIMEOUT
      );
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
      rtems_test_assert(n == sizeof(msg));
    }
  }

  ctx->many_to_one_msg_ops[active_workers - 1][worker_index] = counter;
}

static void test_many_to_one_msg_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  test_context *ctx = (test_context *) base;

  test_fini(
    "ManyToOneMsg",
    &ctx->many_to_one_msg_ops[active_workers - 1][0],
    active_workers
  );
}

static const rtems_test_parallel_job test_jobs[] = {
  {
    .init = test_init,
    .body = test_self_event_body,
    .fini = test_self_event_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_all_to_one_event_body,
    .fini = test_all_to_one_event_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_one_mutex_body,
    .fini = test_one_mutex_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_mutex_body,
    .fini = test_many_mutex_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_self_msg_body,
    .fini = test_self_msg_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_to_one_msg_body,
    .fini = test_many_to_one_msg_fini,
    .cascade = true
  }
};

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  const char *test = "TestTimeFine01";
  size_t i;

  TEST_BEGIN();

  ctx->master = rtems_task_self();

  for (i = 0; i < CPU_COUNT; ++i) {
    rtems_status_code sc;

    sc = rtems_semaphore_create(
      rtems_build_name('T', 'E', 'S', 'T'),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      0,
      &ctx->sema[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_message_queue_create(
      rtems_build_name('T', 'E', 'S', 'T'),
      MSG_COUNT,
      sizeof(test_msg),
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->mq[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  printf("<%s>\n", test);

  rtems_test_parallel(
    &ctx->base,
    NULL,
    &test_jobs[0],
    RTEMS_ARRAY_SIZE(test_jobs)
  );

  printf("</%s>\n", test);

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES CPU_COUNT

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES CPU_COUNT

#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MSG_COUNT, sizeof(test_msg))

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SMP_APPLICATION

#define CONFIGURE_SMP_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
