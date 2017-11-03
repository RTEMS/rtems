/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems.h>
#include <rtems/counter.h>

const char rtems_test_name[] = "TMTIMER 1";

typedef struct {
  size_t cache_line_size;
  size_t data_cache_size;
  int dummy_value;
  volatile int *dummy_data;
  rtems_id first;
} test_context;

static test_context test_instance;

static void prepare_cache(test_context *ctx)
{
  volatile int *data = ctx->dummy_data;
  size_t m = ctx->data_cache_size / sizeof(*data);
  size_t k = ctx->cache_line_size / sizeof(*data);
  size_t j = ctx->dummy_value;
  size_t i;

  for (i = 0; i < m; i += k) {
    data[i] = i + j;
  }

  ctx->dummy_value = i + j;
  rtems_cache_invalidate_entire_instruction();
}

static void never(rtems_id id, void *arg)
{
  rtems_test_assert(0);
}

static rtems_interval interval(size_t i)
{
  rtems_interval d = 50000;

  return i * d + d;
}

static void test_fire_and_cancel(
  test_context *ctx,
  size_t i,
  size_t j,
  const char *name
)
{
  rtems_status_code sc;
  rtems_status_code sc2;
  rtems_counter_ticks a;
  rtems_counter_ticks b;
  rtems_counter_ticks d;
  rtems_id id;
  rtems_interrupt_level level;

  id = ctx->first + i;
  prepare_cache(ctx);

  rtems_interrupt_local_disable(level);
  a = rtems_counter_read();
  sc = rtems_timer_fire_after(id, interval(j), never, NULL);
  sc2 = rtems_timer_cancel(id);
  b = rtems_counter_read();
  rtems_interrupt_local_enable(level);

  d = rtems_counter_difference(b, a);

  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(sc2 == RTEMS_SUCCESSFUL);

  printf(
    "<%s unit=\"ns\">%" PRIu64 "</%s>",
    name,
    rtems_counter_ticks_to_nanoseconds(d),
    name
  );
}

static void test_case(test_context *ctx, size_t j, size_t k)
{
  rtems_status_code sc;
  size_t s;
  size_t t;

  s = j - k;

  for (t = 0; t < s; ++t) {
    size_t u = k + t;

    sc = rtems_timer_fire_after(ctx->first + u, interval(u + 1), never, NULL);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  printf("  <Sample>\n    <ActiveTimers>%zu</ActiveTimers>", j);

  test_fire_and_cancel(ctx, j, 0, "First");
  test_fire_and_cancel(ctx, j, j / 2, "Middle");
  test_fire_and_cancel(ctx, j, j + 1, "Last");

  printf("\n  </Sample>\n");
}

static void test(void)
{
  test_context *ctx = &test_instance;
  rtems_status_code sc;
  rtems_id id;
  rtems_name n;
  size_t j;
  size_t k;
  size_t timer_count;

  ctx->cache_line_size = rtems_cache_get_data_line_size();
  if (ctx->cache_line_size == 0) {
    ctx->cache_line_size = 32;
  }

  ctx->data_cache_size = rtems_cache_get_data_cache_size(0);
  if (ctx->data_cache_size == 0) {
    ctx->data_cache_size = ctx->cache_line_size;
  }

  ctx->dummy_data = malloc(ctx->data_cache_size);
  rtems_test_assert(ctx->dummy_data != NULL);

  n = 1;
  timer_count = 1;

  sc = rtems_timer_create(n, &ctx->first);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  rtems_test_assert(rtems_object_id_get_index(ctx->first) == n);

  while (true) {
    ++n;

    sc = rtems_timer_create(n, &id);
    if (sc != RTEMS_SUCCESSFUL) {
      break;
    }

    rtems_test_assert(rtems_object_id_get_index(id) == n);
    timer_count = n;
  }

  printf("<TMTimer01 timerCount=\"%zu\">\n", timer_count);

  k = 0;
  j = 0;

  while (j < timer_count) {
    test_case(ctx, j, k);
    k = j;
    j = (123 * (j + 1) + 99) / 100;
  }

  test_case(ctx, n - 2, k);

  printf("</TMTimer01>\n");
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 50000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS rtems_resource_unlimited(32)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
