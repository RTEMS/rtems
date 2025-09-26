/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2024 embedded brains GmbH & Co. KG
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

#include <sys/lock.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>

#include <rtems/test-info.h>

const char rtems_test_name[] = "TMFINE 1";

#if defined(RTEMS_SMP)
#define CPU_COUNT 32
#else
#define CPU_COUNT 1
#endif

#define MSG_COUNT 3

typedef struct {
  uint32_t value;
} test_msg;

typedef struct {
  rtems_test_parallel_context base;
  const char *test_sep;
  const char *counter_sep;
  rtems_id master;
  rtems_id sema;
  rtems_id mq[CPU_COUNT];
  uint32_t self_event_ops[CPU_COUNT][CPU_COUNT];
  uint32_t all_to_one_event_ops[CPU_COUNT][CPU_COUNT];
  uint32_t one_mutex_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_mutex_ops[CPU_COUNT][CPU_COUNT];
  uint32_t self_msg_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_to_one_msg_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_sys_lock_mutex_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_classic_ceiling_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_classic_mrsp_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_pthread_spinlock_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_pthread_mutex_inherit_ops[CPU_COUNT][CPU_COUNT];
  uint32_t many_pthread_mutex_protect_ops[CPU_COUNT][CPU_COUNT];
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
  (void) base;
  (void) arg;
  (void) active_workers;

  return test_duration();
}

static void test_fini(
  test_context *ctx,
  const char *type,
  const char *description,
  uint32_t *counters,
  size_t active_workers
)
{
  const char *value_sep;
  size_t i;

  if (active_workers == 1) {
    printf(
      "%s{\n"
      "    \"type\": \"%s\",\n"
      "    \"description\": \"%s\",\n"
      "    \"counter\": [",
      ctx->test_sep,
      type,
      description
    );
    ctx->test_sep = ", ";
    ctx->counter_sep = "\n      ";
  }

  printf("%s[", ctx->counter_sep);
  ctx->counter_sep = "],\n      ";
  value_sep = "";

  for (i = 0; i < active_workers; ++i) {
    printf(
      "%s%" PRIu32,
      value_sep,
      counters[i]
    );
    value_sep = ", ";
  }

  if (active_workers == rtems_scheduler_get_processor_maximum()) {
    printf("]\n    ]\n  }");
  }
}

static void test_self_event_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

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
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "event",
    "Send Event to Self",
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
  (void) arg;

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
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "event",
    "Send Event to One",
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
  (void) arg;

  test_context *ctx = (test_context *) base;
  rtems_id id = ctx->sema;
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
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "contested-mutex",
    "Obtain/Release Contested Classic Inheritance Mutex",
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
  (void) arg;

  test_context *ctx = (test_context *) base;
  rtems_status_code sc;
  rtems_id id;
  uint32_t counter = 0;

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
    0,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->many_mutex_ops[active_workers - 1][worker_index] = counter;

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_many_mutex_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Classic Inheritance Mutex",
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
  (void) arg;

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
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "message",
    "Send Message to Self",
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
  (void) arg;

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
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "message",
    "Send Message to One Receiver",
    &ctx->many_to_one_msg_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_sys_lock_mutex_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  struct _Mutex_Control mtx;
  uint32_t counter = 0;

  _Mutex_Initialize(&mtx);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;

    _Mutex_Acquire(&mtx);
    _Mutex_Release(&mtx);
  }

  ctx->many_sys_lock_mutex_ops[active_workers - 1][worker_index] = counter;
}

static void test_many_sys_lock_mutex_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private <sys/lock.h> Mutex",
    &ctx->many_sys_lock_mutex_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_classic_ceiling_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  rtems_status_code sc;
  rtems_id id;
  uint32_t counter = 0;

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY_CEILING | RTEMS_PRIORITY,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->many_classic_ceiling_ops[active_workers - 1][worker_index] = counter;

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_many_classic_ceiling_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Classic Ceiling Mutex",
    &ctx->many_classic_ceiling_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_classic_mrsp_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  rtems_status_code sc;
  rtems_id id;
  uint32_t counter = 0;

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    1,
    &id
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    rtems_status_code sc;

    ++counter;

    sc = rtems_semaphore_obtain(id, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_semaphore_release(id);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  ctx->many_classic_mrsp_ops[active_workers - 1][worker_index] = counter;

  sc = rtems_semaphore_delete(id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_many_classic_mrsp_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Classic MrsP Mutex",
    &ctx->many_classic_mrsp_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_pthread_spinlock_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  int eno;
  pthread_spinlock_t spin;
  uint32_t counter = 0;

  eno = pthread_spin_init(&spin, 0);
  rtems_test_assert(eno == 0);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;

    pthread_spin_lock(&spin);
    pthread_spin_unlock(&spin);
  }

  ctx->many_pthread_spinlock_ops[active_workers - 1][worker_index] = counter;

  eno = pthread_spin_destroy(&spin);
  rtems_test_assert(eno == 0);
}

static void test_many_pthread_spinlock_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Pthread Spinlock",
    &ctx->many_pthread_spinlock_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_pthread_mutex_inherit_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  int eno;
  pthread_mutexattr_t attr;
  pthread_mutex_t mtx;
  uint32_t counter = 0;

  eno = pthread_mutexattr_init(&attr);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_init(&mtx, &attr);
  rtems_test_assert(eno == 0);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;

    pthread_mutex_lock(&mtx);
    pthread_mutex_unlock(&mtx);
  }

  ctx->many_pthread_mutex_inherit_ops[active_workers - 1][worker_index] =
    counter;

  eno = pthread_mutex_destroy(&mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_destroy(&attr);
  rtems_test_assert(eno == 0);
}

static void test_many_pthread_mutex_inherit_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Pthread Inheritance Mutex",
    &ctx->many_pthread_mutex_inherit_ops[active_workers - 1][0],
    active_workers
  );
}

static void test_many_pthread_mutex_protect_body(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers,
  size_t worker_index
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;
  int eno;
  pthread_mutexattr_t attr;
  pthread_mutex_t mtx;
  uint32_t counter = 0;

  eno = pthread_mutexattr_init(&attr);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_PROTECT);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_setprioceiling(
    &attr,
    sched_get_priority_max(SCHED_FIFO)
  );
  rtems_test_assert(eno == 0);

  eno = pthread_mutex_init(&mtx, &attr);
  rtems_test_assert(eno == 0);

  while (!rtems_test_parallel_stop_job(&ctx->base)) {
    ++counter;

    pthread_mutex_lock(&mtx);
    pthread_mutex_unlock(&mtx);
  }

  ctx->many_pthread_mutex_protect_ops[active_workers - 1][worker_index] =
    counter;

  eno = pthread_mutex_destroy(&mtx);
  rtems_test_assert(eno == 0);

  eno = pthread_mutexattr_destroy(&attr);
  rtems_test_assert(eno == 0);
}

static void test_many_pthread_mutex_protect_fini(
  rtems_test_parallel_context *base,
  void *arg,
  size_t active_workers
)
{
  (void) arg;

  test_context *ctx = (test_context *) base;

  test_fini(
    ctx,
    "private-mutex",
    "Obtain/Release Private Pthread Ceiling Mutex",
    &ctx->many_pthread_mutex_protect_ops[active_workers - 1][0],
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
    .body = test_self_msg_body,
    .fini = test_self_msg_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_to_one_msg_body,
    .fini = test_many_to_one_msg_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_pthread_spinlock_body,
    .fini = test_many_pthread_spinlock_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_sys_lock_mutex_body,
    .fini = test_many_sys_lock_mutex_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_pthread_mutex_inherit_body,
    .fini = test_many_pthread_mutex_inherit_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_mutex_body,
    .fini = test_many_mutex_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_pthread_mutex_protect_body,
    .fini = test_many_pthread_mutex_protect_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_classic_ceiling_body,
    .fini = test_many_classic_ceiling_fini,
    .cascade = true
  }, {
    .init = test_init,
    .body = test_many_classic_mrsp_body,
    .fini = test_many_classic_mrsp_fini,
    .cascade = true
  }
};

static void Init(rtems_task_argument arg)
{
  (void) arg;

  test_context *ctx = &test_instance;
  rtems_status_code sc;
  size_t i;

  TEST_BEGIN();

  ctx->master = rtems_task_self();

  sc = rtems_semaphore_create(
    rtems_build_name('T', 'E', 'S', 'T'),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
    0,
    &ctx->sema
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  for (i = 0; i < CPU_COUNT; ++i) {
    sc = rtems_message_queue_create(
      rtems_build_name('T', 'E', 'S', 'T'),
      MSG_COUNT,
      sizeof(test_msg),
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->mq[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  printf("*** BEGIN OF JSON DATA ***\n[\n  ");

  ctx->test_sep = "";
  rtems_test_parallel(
    &ctx->base,
    NULL,
    &test_jobs[0],
    RTEMS_ARRAY_SIZE(test_jobs)
  );

  printf("\n]\n*** END OF JSON DATA ***\n");

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS CPU_COUNT

#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES (1 + CPU_COUNT)

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES CPU_COUNT

#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MSG_COUNT, sizeof(test_msg))

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY 2

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
