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

#include <threads.h>

#include <rtems/malloc.h>

const char rtems_test_name[] = "SPSTDTHREADS 1";

#define US_PER_TICK 10000

#define EVENT_MTX_LOCK RTEMS_EVENT_0

#define EVENT_MTX_UNLOCK RTEMS_EVENT_1

#define EVENT_CND_WAIT RTEMS_EVENT_2

#define EVENT_CND_TIMEDWAIT RTEMS_EVENT_3

#define EVENT_TSS RTEMS_EVENT_4

typedef struct {
  rtems_id high;
  rtems_id low;
  once_flag once_flag;
  mtx_t mtx;
  cnd_t cnd;
  tss_t tss;
  thrd_t thrd;
  int generation;
} test_context;

static test_context test_instance = {
  .once_flag = ONCE_FLAG_INIT
};

static void next_generation(test_context *ctx)
{
  ++ctx->generation;
}

static void send_event(test_context *ctx, rtems_event_set events)
{
  rtems_status_code sc;

  sc = rtems_event_send(ctx->high, events);
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

static void test_init(test_context *ctx)
{
  int status;

  status = mtx_init(&ctx->mtx, mtx_plain);
  rtems_test_assert(status == thrd_success);

  status = cnd_init(&ctx->cnd);
  rtems_test_assert(status == thrd_success);
}

static void test_destroy(test_context *ctx)
{
  mtx_destroy(&ctx->mtx);
  cnd_destroy(&ctx->cnd);
}

static void once_func(void)
{
  test_context *ctx = &test_instance;

  next_generation(ctx);
}

static void test_once(test_context *ctx)
{
  int gen = ctx->generation;

  call_once(&ctx->once_flag, once_func);
  rtems_test_assert(ctx->generation == gen + 1);

  call_once(&ctx->once_flag, once_func);
  rtems_test_assert(ctx->generation == gen + 1);
}

static void test_mtx(test_context *ctx)
{
  mtx_t *mtx = &ctx->mtx;
  int gen = ctx->generation;
  struct timespec to;
  int status;

  status = mtx_trylock(mtx);
  rtems_test_assert(status == thrd_success);

  status = mtx_lock(mtx);
  rtems_test_assert(status == thrd_success);

  get_abs_timeout(&to);
  status = mtx_timedlock(mtx, &to);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);

  send_event(ctx, EVENT_MTX_LOCK);
  rtems_test_assert(ctx->generation == gen + 1);

  status = mtx_trylock(mtx);
  rtems_test_assert(status == thrd_busy);

  memset(&to, 0xff, sizeof(to));
  status = mtx_timedlock(mtx, &to);
  rtems_test_assert(status == thrd_error);

  get_abs_timeout(&to);
  status = mtx_timedlock(mtx, &to);
  rtems_test_assert(status == thrd_timedout);

  send_event(ctx, EVENT_MTX_UNLOCK);
  rtems_test_assert(ctx->generation == gen + 2);
}

static void test_cnd(test_context *ctx)
{
  cnd_t *cnd = &ctx->cnd;
  mtx_t *mtx = &ctx->mtx;
  int gen = ctx->generation;
  struct timespec to;
  int status;

  send_event(ctx, EVENT_CND_WAIT);
  rtems_test_assert(ctx->generation == gen + 1);

  status = mtx_lock(mtx);
  rtems_test_assert(status == thrd_success);

  status = cnd_signal(cnd);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);
  rtems_test_assert(ctx->generation == gen + 2);

  send_event(ctx, EVENT_CND_WAIT);
  rtems_test_assert(ctx->generation == gen + 3);

  status = mtx_lock(mtx);
  rtems_test_assert(status == thrd_success);

  status = cnd_broadcast(cnd);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);
  rtems_test_assert(ctx->generation == gen + 4);

  status = mtx_lock(mtx);
  rtems_test_assert(status == thrd_success);

  memset(&to, 0xff, sizeof(to));
  status = cnd_timedwait(cnd, mtx, &to);
  rtems_test_assert(status == thrd_error);

  get_abs_timeout(&to);
  status = cnd_timedwait(cnd, mtx, &to);
  rtems_test_assert(status == thrd_timedout);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);

  send_event(ctx, EVENT_CND_TIMEDWAIT);
  rtems_test_assert(ctx->generation == gen + 5);

  status = mtx_lock(mtx);
  rtems_test_assert(status == thrd_success);

  status = cnd_signal(cnd);
  rtems_test_assert(status == thrd_success);

  status = mtx_unlock(mtx);
  rtems_test_assert(status == thrd_success);
  rtems_test_assert(ctx->generation == gen + 6);
}

static int tss_val = TSS_DTOR_ITERATIONS;

static void tss_dtor(void *val)
{
  test_context *ctx = &test_instance;

  rtems_test_assert(val == &tss_val);
  next_generation(ctx);
}

static void test_tss(test_context *ctx)
{
  tss_dtor_t dtor = tss_dtor;
  int gen = ctx->generation;
  int status;

  status = tss_create(&ctx->tss, dtor);
  rtems_test_assert(status == thrd_success);

  send_event(ctx, EVENT_TSS);
  rtems_test_assert(ctx->generation == gen + 1);

  tss_delete(ctx->tss);
}

static int thrd(void *arg)
{
  thrd_exit(123);
}

static void test_thrd(test_context *ctx)
{
  thrd_start_t thrd_start = thrd;
  int status;
  int exit_status;
  struct timespec duration;
  struct timespec remaining;
  void *greedy;

  rtems_test_assert(thrd_equal(rtems_task_self(), thrd_current()));

  thrd_yield();

  memset(&duration, 0, sizeof(duration));
  duration.tv_nsec = 1;
  thrd_sleep(&duration, &remaining);
  rtems_test_assert(remaining.tv_sec == 0);
  rtems_test_assert(remaining.tv_nsec == 0);

  greedy = rtems_heap_greedy_allocate(NULL, 0);
  status = thrd_create(&ctx->thrd, thrd_start, ctx);
  rtems_test_assert(status == thrd_nomem);
  rtems_heap_greedy_free(greedy);

  status = thrd_create(&ctx->thrd, thrd_start, ctx);
  rtems_test_assert(status == thrd_success);

  status = thrd_create(&ctx->thrd, thrd_start, ctx);
  rtems_test_assert(status == thrd_error);

  exit_status = 0;
  status = thrd_join(ctx->thrd, &exit_status);
  rtems_test_assert(status == thrd_success);
  rtems_test_assert(exit_status == 123);

  status = thrd_detach(thrd_current());
  rtems_test_assert(status == thrd_success);

  status = thrd_detach(11235);
  rtems_test_assert(status == thrd_error);
}

static void high_task(rtems_task_argument idx)
{
  test_context *ctx = &test_instance;

  while (true) {
    rtems_event_set events;
    rtems_status_code sc;
    int status;

    sc = rtems_event_receive(
      RTEMS_ALL_EVENTS,
      RTEMS_EVENT_ANY | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    if ((events & EVENT_MTX_LOCK) != 0) {
      status = mtx_lock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);
    }

    if ((events & EVENT_MTX_UNLOCK) != 0) {
      status = mtx_unlock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);
    }

    if ((events & EVENT_CND_WAIT) != 0) {
      status = mtx_lock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);

      status = cnd_wait(&ctx->cnd, &ctx->mtx);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);

      status = mtx_unlock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
    }

    if ((events & EVENT_CND_TIMEDWAIT) != 0) {
      struct timespec to;

      status = mtx_lock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);

      get_abs_timeout(&to);
      status = cnd_timedwait(&ctx->cnd, &ctx->mtx, &to);
      rtems_test_assert(status == thrd_success);
      next_generation(ctx);

      status = mtx_unlock(&ctx->mtx);
      rtems_test_assert(status == thrd_success);
    }

    if ((events & EVENT_TSS) != 0) {
      void *val;

      status = tss_set(ctx->tss, &tss_val);
      rtems_test_assert(status == thrd_success);

      val = tss_get(ctx->tss);
      rtems_test_assert(val == &tss_val);

      rtems_task_exit();
      rtems_test_assert(0);
    }
  }
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;

  test_init(ctx);

  ctx->low = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name('H', 'I', 'G', 'H'),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->high
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_start(ctx->high, high_task, 0);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  test_once(ctx);
  test_mtx(ctx);
  test_cnd(ctx);
  test_tss(ctx);
  test_thrd(ctx);
  test_destroy(ctx);
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
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 4

#define CONFIGURE_MAXIMUM_POSIX_KEYS 1
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 1
#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_SCHEDULER_NAME rtems_build_name('b', 'l', 'u', 'e')

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
