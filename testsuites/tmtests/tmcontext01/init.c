/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
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

#include <rtems/counter.h>
#include <rtems.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <alloca.h>

#include "tmacros.h"

#define FUNCTION_LEVELS 16

#define SAMPLES 123

#define CPU_COUNT 32

const char rtems_test_name[] = "TMCONTEXT 1";

static rtems_counter_ticks t[SAMPLES];

static size_t cache_line_size;

static size_t data_size;

static volatile int *main_data;

static Context_Control ctx;

static int dirty_data_cache(volatile int *data, size_t n, size_t clsz, int j)
{
  size_t m = n / sizeof(*data);
  size_t k = clsz / sizeof(*data);
  size_t i;

  for (i = 0; i < m; i += k) {
    data[i] = i + j;
  }

  return i + j;
}

static __attribute__((__noipa__)) void call_at_level(
  int start,
  int fl,
  int s
)
{
#if defined(__sparc__)
  if (fl == start) {
    /* Flush register windows */
    __asm__ volatile ("ta 3" : : : "memory");
  }
#endif

  if (fl > 0) {
    call_at_level(
      start,
      fl - 1,
      s
    );
    __asm__ volatile ("" : : : "memory");
  } else {
    char *volatile space;
    rtems_counter_ticks a;
    rtems_counter_ticks b;

    a = rtems_counter_read();

    /* Ensure that we use an untouched stack area */
    space = alloca(1024);
    (void) space;

    _Context_Switch(&ctx, &ctx);

    b = rtems_counter_read();
    t[s] = rtems_counter_difference(b, a);
  }
}

static void load_task(rtems_task_argument arg)
{
  volatile int *load_data = (volatile int *) arg;
  size_t n = data_size;
  size_t clsz = cache_line_size;
  int j = (int) rtems_scheduler_get_processor();

  while (true) {
    j = dirty_data_cache(load_data, n, clsz, j);
  }
}

static int cmp(const void *ap, const void *bp)
{
  const rtems_counter_ticks *a = ap;
  const rtems_counter_ticks *b = bp;

  return *a - *b;
}

static void sort_t(void)
{
  qsort(&t[0], SAMPLES, sizeof(t[0]), cmp);
}

static __attribute__((__noipa__)) void test_by_function_level(int fl, bool dirty)
{
  RTEMS_INTERRUPT_LOCK_DECLARE(, lock)
  rtems_interrupt_lock_context lock_context;
  int s;
  uint64_t min;
  uint64_t q1;
  uint64_t q2;
  uint64_t q3;
  uint64_t max;

  rtems_interrupt_lock_initialize(&lock, "test");
  rtems_interrupt_lock_acquire(&lock, &lock_context);

  for (s = 0; s < SAMPLES; ++s) {
    if (dirty) {
      dirty_data_cache(main_data, data_size, cache_line_size, fl);
      rtems_cache_invalidate_entire_instruction();
    }

    call_at_level(fl, fl, s);
  }

  rtems_interrupt_lock_release(&lock, &lock_context);
  rtems_interrupt_lock_destroy(&lock);

  sort_t();

  min = t[0];
  q1 = t[(1 * SAMPLES) / 4];
  q2 = t[SAMPLES / 2];
  q3 = t[(3 * SAMPLES) / 4];
  max = t[SAMPLES - 1];

  printf(
    "%s\n      [%" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 "]",
    fl == 0 ? "" : ",",
    rtems_counter_ticks_to_nanoseconds(min),
    rtems_counter_ticks_to_nanoseconds(q1),
    rtems_counter_ticks_to_nanoseconds(q2),
    rtems_counter_ticks_to_nanoseconds(q3),
    rtems_counter_ticks_to_nanoseconds(max)
  );
}

static void test(bool first, bool dirty, uint32_t load)
{
  int fl;

  printf(
    "\n  %s{\n"
    "    \"environment\": \"",
    first ? "" : "}, "
  );

  if (dirty) {
    if (load > 0) {
      printf("Load/%" PRIu32 "", load);
    } else {
      printf("DirtyCache");
    }
  } else {
    printf("HotCache");
  }

  printf(
    "\",\n"
    "    \"stats-by-function-nest-level\": ["
  );

  for (fl = 0; fl < FUNCTION_LEVELS; ++fl) {
    test_by_function_level(fl, dirty);
  }

  printf(
    "\n    ]"
  );
}

static void Init(rtems_task_argument arg)
{
  uint32_t load = 0;

  TEST_BEGIN();

  printf("*** BEGIN OF JSON DATA ***\n[");

  cache_line_size = rtems_cache_get_data_line_size();
  if (cache_line_size == 0) {
    cache_line_size = 32;
  }

  data_size = rtems_cache_get_data_cache_size(0);
  if (data_size == 0) {
    data_size = cache_line_size;
  }

  main_data = malloc(data_size);
  rtems_test_assert(main_data != NULL);

  test(true, false, load);
  test(false, true, load);

  for (load = 1; load < rtems_scheduler_get_processor_maximum(); ++load) {
    rtems_status_code sc;
    rtems_id id;
    volatile int *load_data = NULL;

    load_data = malloc(data_size);
    if (load_data == NULL) {
      load_data = main_data;
    }

    sc = rtems_task_create(
      rtems_build_name('L', 'O', 'A', 'D'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    sc = rtems_task_start(id, load_task, (rtems_task_argument) load_data);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);

    test(false, true, load);
  }

  printf("\n  }\n]\n*** END OF JSON DATA ***\n");

  TEST_END();
  rtems_test_exit(0);
}

/*
 * Do not use a clock driver, since this will disturb the test in the "normal"
 * environment.
 */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS (1 + CPU_COUNT)

#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)

#define CONFIGURE_MAXIMUM_PROCESSORS CPU_COUNT

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
