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

#include <sys/lock.h>
#include <string.h>
#include <time.h>

const char rtems_test_name[] = "SPSYSLOCK 1";

#define US_PER_TICK 10000

#define EVENT_MTX_ACQUIRE RTEMS_EVENT_0

#define EVENT_MTX_RELEASE RTEMS_EVENT_1

#define EVENT_MTX_PRIO_INV RTEMS_EVENT_2

#define EVENT_MTX_DEADLOCK RTEMS_EVENT_3

#define EVENT_REC_MTX_ACQUIRE RTEMS_EVENT_4

#define EVENT_REC_MTX_RELEASE RTEMS_EVENT_5

#define EVENT_REC_MTX_PRIO_INV RTEMS_EVENT_6

typedef struct {
  rtems_id high[2];
  rtems_id mid;
  rtems_id low;
  struct _Mutex_Control mtx;
  struct _Mutex_recursive_Control rec_mtx;
  int generation[2];
  int current_generation[2];
} test_context;

static test_context test_instance;

static int generation(test_context *ctx, size_t idx)
{
  return ++ctx->current_generation[idx];
}

static void send_event(test_context *ctx, size_t idx, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->high[idx], events);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void get_abs_timeout(struct timespec *to)
{
  int rv;

  rv = clock_gettime(CLOCK_REALTIME, to);
  rtems_test_assert(rv == 0);

  to->tv_nsec += 2 * US_PER_TICK * 1000;
  if (to->tv_nsec >= 1000000000) {
    ++to->tv_sec;
    to->tv_nsec -= 1000000000;
  }
}

static void test_initialization(test_context *ctx)
{
  struct _Mutex_Control mtx = _MUTEX_INITIALIZER;
  struct _Mutex_recursive_Control rec_mtx = _MUTEX_RECURSIVE_INITIALIZER;

  _Mutex_Initialize(&ctx->mtx);
  _Mutex_recursive_Initialize(&ctx->rec_mtx);

  rtems_test_assert(memcmp(&mtx, &ctx->mtx, sizeof(mtx)) == 0);
  rtems_test_assert(memcmp(&rec_mtx, &ctx->rec_mtx, sizeof(rec_mtx)) == 0);

  _Mutex_Destroy(&mtx);
  _Mutex_recursive_Destroy(&rec_mtx);
}

static void test_recursive_acquire_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  int success;

  success = _Mutex_Try_acquire(mtx);
  rtems_test_assert(success == 1);

  success = _Mutex_Try_acquire(mtx);
  rtems_test_assert(success == 0);

  _Mutex_Release(mtx);

  success = _Mutex_Try_acquire(mtx);
  rtems_test_assert(success == 1);

  _Mutex_Release(mtx);

  _Mutex_Acquire(mtx);

  success = _Mutex_Try_acquire(mtx);
  rtems_test_assert(success == 0);

  _Mutex_Release(mtx);

  send_event(ctx, idx, EVENT_MTX_ACQUIRE);

  success = _Mutex_Try_acquire(mtx);
  rtems_test_assert(success == 0);

  send_event(ctx, idx, EVENT_MTX_RELEASE);
}

static void test_recursive_acquire_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  int success;

  success = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(success == 1);

  _Mutex_recursive_Acquire(mtx);

  success = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(success == 1);

  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);

  send_event(ctx, idx, EVENT_REC_MTX_ACQUIRE);

  success = _Mutex_recursive_Try_acquire(mtx);
  rtems_test_assert(success == 0);

  send_event(ctx, idx, EVENT_REC_MTX_RELEASE);
}

static void test_prio_acquire_order(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t a = 0;
  size_t b = 1;
  int gen_a;
  int gen_b;

  _Mutex_Acquire(mtx);

  gen_a = ctx->generation[a];
  gen_b = ctx->generation[b];

  send_event(ctx, b, EVENT_MTX_ACQUIRE);
  send_event(ctx, a, EVENT_MTX_ACQUIRE);

  rtems_test_assert(ctx->generation[a] == gen_a);
  rtems_test_assert(ctx->generation[b] == gen_b);

  _Mutex_Release(mtx);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b);

  send_event(ctx, a, EVENT_MTX_RELEASE);

  rtems_test_assert(ctx->generation[a] == gen_a + 1);
  rtems_test_assert(ctx->generation[b] == gen_b + 1);

  send_event(ctx, b, EVENT_MTX_RELEASE);
}

static void test_prio_inv_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  int gen;

  _Mutex_Acquire(mtx);
  gen = ctx->generation[idx];
  send_event(ctx, idx, EVENT_MTX_PRIO_INV);
  rtems_test_assert(ctx->generation[idx] == gen);
  _Mutex_Release(mtx);
  rtems_test_assert(ctx->generation[idx] == gen + 1);
}

static void test_prio_inv_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  int gen;

  _Mutex_recursive_Acquire(mtx);
  gen = ctx->generation[idx];
  send_event(ctx, idx, EVENT_REC_MTX_PRIO_INV);
  rtems_test_assert(ctx->generation[idx] == gen);
  _Mutex_recursive_Release(mtx);
  rtems_test_assert(ctx->generation[idx] == gen + 1);
}

static void test_mtx_timeout_normal(test_context *ctx)
{
  struct _Mutex_Control *mtx = &ctx->mtx;
  size_t idx = 0;
  struct timespec to;
  int eno;

  eno = _Mutex_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  _Mutex_Release(mtx);

  send_event(ctx, idx, EVENT_MTX_ACQUIRE);

  memset(&to, 0x00, sizeof(to));
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Mutex_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);

  send_event(ctx, idx, EVENT_MTX_RELEASE);
}

static void test_mtx_timeout_recursive(test_context *ctx)
{
  struct _Mutex_recursive_Control *mtx = &ctx->rec_mtx;
  size_t idx = 0;
  struct timespec to;
  int eno;

  eno = _Mutex_recursive_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  eno = _Mutex_recursive_Acquire_timed(mtx, NULL);
  rtems_test_assert(eno == 0);
  _Mutex_recursive_Release(mtx);
  _Mutex_recursive_Release(mtx);

  send_event(ctx, idx, EVENT_REC_MTX_ACQUIRE);

  memset(&to, 0x00, sizeof(to));
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);
  memset(&to, 0xff, sizeof(to));
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == EINVAL);
  get_abs_timeout(&to);
  eno = _Mutex_recursive_Acquire_timed(mtx, &to);
  rtems_test_assert(eno == ETIMEDOUT);

  send_event(ctx, idx, EVENT_REC_MTX_RELEASE);
}

static void mid_task(rtems_task_argument arg)
{
  rtems_test_assert(0);
}

static void high_task(rtems_task_argument idx)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  if (idx == 0) {
    sc = rtems_task_start(ctx->mid, mid_task, 0);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_suspend(ctx->mid);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  while (true) {
    rtems_event_set events;

    sc = rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    if ((events & EVENT_MTX_ACQUIRE) != 0) {
      _Mutex_Acquire(&ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
    }

    if ((events & EVENT_MTX_RELEASE) != 0) {
      _Mutex_Release(&ctx->mtx);
    }

    if ((events & EVENT_MTX_PRIO_INV) != 0) {
      sc = rtems_task_resume(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      _Mutex_Acquire(&ctx->mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_Release(&ctx->mtx);

      sc = rtems_task_suspend(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }

    if ((events & EVENT_MTX_DEADLOCK) != 0) {
      struct _Mutex_Control dead = _MUTEX_INITIALIZER;

      _Mutex_Acquire(&dead);
      _Mutex_Acquire(&dead);
    }

    if ((events & EVENT_REC_MTX_ACQUIRE) != 0) {
      _Mutex_recursive_Acquire(&ctx->rec_mtx);
    }

    if ((events & EVENT_REC_MTX_RELEASE) != 0) {
      _Mutex_recursive_Release(&ctx->rec_mtx);
    }

    if ((events & EVENT_REC_MTX_PRIO_INV) != 0) {
      sc = rtems_task_resume(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);

      _Mutex_recursive_Acquire(&ctx->rec_mtx);
      ctx->generation[idx] = generation(ctx, idx);
      _Mutex_recursive_Release(&ctx->rec_mtx);

      sc = rtems_task_suspend(ctx->mid);
      rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    }
  }
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  test_initialization(ctx);

  ctx->low = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name('M', 'I', 'D', ' '),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->mid
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '0'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high[0]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high[0], high_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', '1'),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high[1]
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high[1], high_task, 1);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_recursive_acquire_normal(ctx);
  test_recursive_acquire_recursive(ctx);
  test_prio_acquire_order(ctx);
  test_prio_inv_normal(ctx);
  test_prio_inv_recursive(ctx);
  test_mtx_timeout_normal(ctx);
  test_mtx_timeout_recursive(ctx);

  send_event(ctx, 0, EVENT_MTX_DEADLOCK);

  _Mutex_Destroy(&ctx->mtx);
  _Mutex_recursive_Destroy(&ctx->rec_mtx);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK US_PER_TICK

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
