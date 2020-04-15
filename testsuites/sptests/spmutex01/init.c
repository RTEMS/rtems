/*
 * Copyright (c) 2015, 2017 embedded brains GmbH.  All rights reserved.
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

#include <errno.h>
#include <pthread.h>
#include <setjmp.h>
#include <threads.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPMUTEX 1";

#define TASK_COUNT 5

#define MTX_COUNT 3

typedef enum {
  REQ_WAKE_UP_MASTER = RTEMS_EVENT_0,
  REQ_WAKE_UP_HELPER = RTEMS_EVENT_1,
  REQ_MTX_0_OBTAIN = RTEMS_EVENT_2,
  REQ_MTX_0_OBTAIN_UNSATISFIED = RTEMS_EVENT_3,
  REQ_MTX_0_RELEASE = RTEMS_EVENT_4,
  REQ_MTX_1_OBTAIN = RTEMS_EVENT_5,
  REQ_MTX_1_OBTAIN_TIMEOUT = RTEMS_EVENT_6,
  REQ_MTX_1_RELEASE = RTEMS_EVENT_7,
  REQ_MTX_2_OBTAIN = RTEMS_EVENT_8,
  REQ_MTX_2_RELEASE = RTEMS_EVENT_9,
  REQ_MTX_C11_OBTAIN = RTEMS_EVENT_10,
  REQ_MTX_C11_RELEASE = RTEMS_EVENT_11,
  REQ_MTX_POSIX_OBTAIN = RTEMS_EVENT_12,
  REQ_MTX_POSIX_RELEASE = RTEMS_EVENT_13
} request_id;

typedef enum {
  M,
  A_1,
  A_2_0,
  A_2_1,
  H,
  NONE
} task_id;

typedef enum {
  MTX_0,
  MTX_1,
  MTX_2
} mutex_id;

typedef struct {
  rtems_id mtx[MTX_COUNT];
  mtx_t mtx_c11;
  pthread_mutex_t mtx_posix;
  rtems_id tasks[TASK_COUNT];
  int generation[TASK_COUNT];
  int expected_generation[TASK_COUNT];
  jmp_buf deadlock_return_context;
} test_context;

static test_context test_instance;

static void start_task(
  test_context *ctx,
  task_id id,
  rtems_task_entry entry,
  rtems_task_priority prio
)
{
  rtems_status_code sc;

  sc = rtems_task_create(
    rtems_build_name('T', 'A', 'S', 'K'),
    prio,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->tasks[id]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->tasks[id], entry, id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void send_event(test_context *ctx, task_id id, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->tasks[id], events);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void wait(void)
{
  rtems_status_code sc;

  sc = rtems_task_wake_after(4);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static rtems_event_set wait_for_events(void)
{
  rtems_event_set events;
  rtems_status_code sc;

  sc = rtems_event_receive(
    RTEMS_ALL_EVENTS,
    RTEMS_EVENT_ANY | RTEMS_WAIT,
    RTEMS_NO_TIMEOUT,
    &events
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  return events;
}

static void sync_with_helper(test_context *ctx)
{
  rtems_event_set events;

  send_event(ctx, H, REQ_WAKE_UP_HELPER);
  events = wait_for_events();
  rtems_test_assert(events == REQ_WAKE_UP_MASTER);
}

static void request(test_context *ctx, task_id id, request_id req)
{
  send_event(ctx, id, req);
  sync_with_helper(ctx);
}

static void obtain_timeout(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx[id], RTEMS_WAIT, 2);
  rtems_test_assert(sc == RTEMS_TIMEOUT);
}

static void obtain_unsatisfied(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx[id], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void obtain(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx[id], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void deadlock_obtain(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_obtain(ctx->mtx[id], RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

static void release(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_release(ctx->mtx[id]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void flush(test_context *ctx, mutex_id id)
{
  rtems_status_code sc;

  sc = rtems_semaphore_flush(ctx->mtx[id]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void obtain_c11(test_context *ctx)
{
  int status;

  status = mtx_lock(&ctx->mtx_c11);
  rtems_test_assert(status == thrd_success);
}

static void deadlock_obtain_c11(test_context *ctx)
{
  if (setjmp(ctx->deadlock_return_context) == 0) {
    (void) mtx_lock(&ctx->mtx_c11);
  }
}

static void release_c11(test_context *ctx)
{
  int status;

  status = mtx_unlock(&ctx->mtx_c11);
  rtems_test_assert(status == thrd_success);
}

static void obtain_posix(test_context *ctx)
{
  int error;

  error = pthread_mutex_lock(&ctx->mtx_posix);
  rtems_test_assert(error == 0);
}

static void deadlock_obtain_posix(test_context *ctx)
{
  int error;

  error = pthread_mutex_lock(&ctx->mtx_posix);
  rtems_test_assert(error == EDEADLK);
}

static void release_posix(test_context *ctx)
{
  int error;

  error = pthread_mutex_unlock(&ctx->mtx_posix);
  rtems_test_assert(error == 0);
}

static void check_generations(test_context *ctx, task_id a, task_id b)
{
  size_t i;

  if (a != NONE) {
    ++ctx->expected_generation[a];
  }

  if (b != NONE) {
    ++ctx->expected_generation[b];
  }

  for (i = 0; i < TASK_COUNT; ++i) {
    rtems_test_assert(ctx->generation[i] == ctx->expected_generation[i]);
  }
}

static void assert_prio(
  test_context *ctx,
  task_id id,
  rtems_task_priority expected
)
{
  rtems_task_priority actual;
  rtems_status_code sc;

  sc = rtems_task_set_priority(
    ctx->tasks[id],
    RTEMS_CURRENT_PRIORITY,
    &actual
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(expected == actual);
}

static void change_prio(
  test_context *ctx,
  task_id id,
  rtems_task_priority prio
)
{
  rtems_status_code sc;

  sc = rtems_task_set_priority(
    ctx->tasks[id],
    prio,
    &prio
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void helper(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;

  while (true) {
    rtems_event_set events = wait_for_events();
    rtems_test_assert(events == REQ_WAKE_UP_HELPER);
    send_event(ctx, M, REQ_WAKE_UP_MASTER);
  }
}

static void worker(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  task_id id = arg;

  while (true) {
    rtems_event_set events = wait_for_events();

    if ((events & REQ_MTX_0_OBTAIN) != 0) {
      obtain(ctx, MTX_0);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_0_OBTAIN_UNSATISFIED) != 0) {
      obtain_unsatisfied(ctx, MTX_0);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_0_RELEASE) != 0) {
      release(ctx, MTX_0);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_1_OBTAIN) != 0) {
      obtain(ctx, MTX_1);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_1_OBTAIN_TIMEOUT) != 0) {
      obtain_timeout(ctx, MTX_1);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_1_RELEASE) != 0) {
      release(ctx, MTX_1);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_2_OBTAIN) != 0) {
      obtain(ctx, MTX_2);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_2_RELEASE) != 0) {
      release(ctx, MTX_2);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_C11_OBTAIN) != 0) {
      obtain_c11(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_C11_RELEASE) != 0) {
      release_c11(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_POSIX_OBTAIN) != 0) {
      obtain_posix(ctx);
      ++ctx->generation[id];
    }

    if ((events & REQ_MTX_POSIX_RELEASE) != 0) {
      release_posix(ctx);
      ++ctx->generation[id];
    }
  }
}

static void set_up(test_context *ctx)
{
  rtems_status_code sc;
  int status;
  size_t i;
  int error;
  pthread_mutexattr_t attr;

  ctx->tasks[M] = rtems_task_self();
  start_task(ctx, A_1, worker, 1);
  start_task(ctx, A_2_0, worker, 2);
  start_task(ctx, A_2_1, worker, 2);
  start_task(ctx, H, helper, 3);

  for (i = 0; i < MTX_COUNT; ++i) {
    sc = rtems_semaphore_create(
      rtems_build_name(' ', 'M', 'T', 'X'),
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
      0,
      &ctx->mtx[i]
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  status = mtx_init(&ctx->mtx_c11, mtx_plain);
  rtems_test_assert(status == thrd_success);

  error = pthread_mutexattr_init(&attr);
  rtems_test_assert(error == 0);

  error = pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
  rtems_test_assert(error == 0);

  error = pthread_mutex_init(&ctx->mtx_posix, &attr);
  rtems_test_assert(error == 0);

  error = pthread_mutexattr_destroy(&attr);
  rtems_test_assert(error == 0);
}

static void test_inherit(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  change_prio(ctx, A_1, 2);
  assert_prio(ctx, M, 2);
  change_prio(ctx, A_1, 3);
  assert_prio(ctx, M, 3);
  change_prio(ctx, A_1, 4);
  assert_prio(ctx, M, 3);
  change_prio(ctx, A_1, 1);
  assert_prio(ctx, M, 1);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_inherit_fifo_for_equal_priority(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  request(ctx, A_2_0, REQ_MTX_0_OBTAIN);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  request(ctx, A_2_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  assert_prio(ctx, A_1, 1);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, A_2_0);
  request(ctx, A_2_0, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_2_0, A_2_1);
  request(ctx, A_2_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_2_1, NONE);
}

static void test_inherit_nested_vertical(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  obtain(ctx, MTX_1);
  request(ctx, A_1, REQ_MTX_1_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  release(ctx, MTX_1);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  request(ctx, A_1, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_1, NONE);
  release(ctx, MTX_0);
}

static void test_inherit_nested_vertical_timeout(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  obtain(ctx, MTX_1);
  request(ctx, A_1, REQ_MTX_1_OBTAIN_TIMEOUT);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  wait();
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  release(ctx, MTX_1);
  release(ctx, MTX_0);
}

static void test_inherit_nested_horizontal(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  request(ctx, A_2_0, REQ_MTX_1_OBTAIN);
  check_generations(ctx, A_2_0, NONE);
  request(ctx, A_2_0, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 2);
  request(ctx, A_1, REQ_MTX_1_OBTAIN_TIMEOUT);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, A_2_0, 1);
  assert_prio(ctx, M, 1);
  wait();
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, A_2_0, 2);
  assert_prio(ctx, M, 2);
  request(ctx, A_1, REQ_MTX_1_OBTAIN);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, A_2_0, 1);
  assert_prio(ctx, M, 1);
  change_prio(ctx, A_1, 2);
  assert_prio(ctx, M, 2);
  change_prio(ctx, A_1, 3);
  assert_prio(ctx, M, 2);
  change_prio(ctx, A_2_0, 3);
  assert_prio(ctx, M, 3);
  change_prio(ctx, A_2_0, 2);
  assert_prio(ctx, M, 2);
  change_prio(ctx, A_1, 1);
  assert_prio(ctx, M, 1);
  release(ctx, MTX_0);
  check_generations(ctx, A_2_0, NONE);
  assert_prio(ctx, A_2_0, 1);
  assert_prio(ctx, M, 3);
  request(ctx, A_2_0, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_2_0, NONE);
  assert_prio(ctx, A_2_0, 1);
  request(ctx, A_2_0, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_1, A_2_0);
  assert_prio(ctx, A_2_0, 2);
  request(ctx, A_1, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_inherit_flush(test_context *ctx)
{
  assert_prio(ctx, M, 3);
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_0_OBTAIN_UNSATISFIED);
  check_generations(ctx, NONE, NONE);
  assert_prio(ctx, M, 1);
  flush(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  assert_prio(ctx, M, 3);
  release(ctx, MTX_0);
}

static void test_deadlock_two_classic(test_context *ctx)
{
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_1_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain(ctx, MTX_1);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_deadlock_three_classic(test_context *ctx)
{
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_1_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_2_0, REQ_MTX_2_OBTAIN);
  check_generations(ctx, A_2_0, NONE);
  request(ctx, A_2_0, REQ_MTX_1_OBTAIN);
  check_generations(ctx, NONE, NONE);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain(ctx, MTX_2);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_1, A_2_0);
  request(ctx, A_2_0, REQ_MTX_2_RELEASE);
  check_generations(ctx, A_2_0, NONE);
  request(ctx, A_2_0, REQ_MTX_1_RELEASE);
  check_generations(ctx, A_2_0, NONE);
}

static void test_deadlock_c11_and_classic(test_context *ctx)
{
  obtain_c11(ctx);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_C11_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain(ctx, MTX_0);
  release_c11(ctx);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_C11_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_deadlock_classic_and_c11(test_context *ctx)
{
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_C11_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain_c11(ctx);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_C11_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_deadlock_posix_and_classic(test_context *ctx)
{
  obtain_posix(ctx);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_POSIX_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain(ctx, MTX_0);
  release_posix(ctx);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_POSIX_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void test_deadlock_classic_and_posix(test_context *ctx)
{
  obtain(ctx, MTX_0);
  request(ctx, A_1, REQ_MTX_POSIX_OBTAIN);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_OBTAIN);
  check_generations(ctx, NONE, NONE);
  deadlock_obtain_posix(ctx);
  release(ctx, MTX_0);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_0_RELEASE);
  check_generations(ctx, A_1, NONE);
  request(ctx, A_1, REQ_MTX_POSIX_RELEASE);
  check_generations(ctx, A_1, NONE);
}

static void tear_down(test_context *ctx)
{
  rtems_status_code sc;
  size_t i;
  int error;

  for (i = 1; i < TASK_COUNT; ++i) {
    sc = rtems_task_delete(ctx->tasks[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  for (i = 0; i < MTX_COUNT; ++i) {
    sc = rtems_semaphore_delete(ctx->mtx[i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  mtx_destroy(&ctx->mtx_c11);

  error = pthread_mutex_destroy(&ctx->mtx_posix);
  rtems_test_assert(error == 0);
}

static void Init(rtems_task_argument arg)
{
  test_context *ctx = &test_instance;
  rtems_resource_snapshot snapshot;

  TEST_BEGIN();
  rtems_resource_snapshot_take(&snapshot);

  set_up(ctx);
  test_inherit(ctx);
  test_inherit_fifo_for_equal_priority(ctx);
  test_inherit_nested_vertical(ctx);
  test_inherit_nested_vertical_timeout(ctx);
  test_inherit_nested_horizontal(ctx);
  test_inherit_flush(ctx);
  test_deadlock_two_classic(ctx);
  test_deadlock_three_classic(ctx);
  test_deadlock_c11_and_classic(ctx);
  test_deadlock_classic_and_c11(ctx);
  test_deadlock_posix_and_classic(ctx);
  test_deadlock_classic_and_posix(ctx);
  tear_down(ctx);

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  TEST_END();
  rtems_test_exit(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{

  if (
    source == INTERNAL_ERROR_CORE
      && !always_set_to_false
      && error == INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK
  ) {
    test_context *ctx = &test_instance;

    longjmp(ctx->deadlock_return_context, 1);
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS TASK_COUNT

#define CONFIGURE_MAXIMUM_SEMAPHORES 3

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 3

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
