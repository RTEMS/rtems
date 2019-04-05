/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
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

static volatile int prevent_optimization;

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

static int call_at_level(
  int start,
  int fl,
  int s,
  bool dirty
)
{
  int prevent_optimization;;

  prevent_optimization = start + fl;

  if (fl == start) {
    /*
     * Some architectures like the SPARC have register windows.  A side-effect
     * of this context switch is that we start with a fresh window set.  On
     * architectures like ARM or PowerPC this context switch has no effect.
     */
    _Context_Switch(&ctx, &ctx);
  }

  if (fl > 0) {
    call_at_level(
      start,
      fl - 1,
      s,
      dirty
    );
  } else {
    char *volatile space;
    rtems_counter_ticks a;
    rtems_counter_ticks b;

    if (dirty) {
      dirty_data_cache(main_data, data_size, cache_line_size, fl);
      rtems_cache_invalidate_entire_instruction();
    }

    a = rtems_counter_read();

    /* Ensure that we use an untouched stack area */
    space = alloca(1024);
    (void) space;

    _Context_Switch(&ctx, &ctx);

    b = rtems_counter_read();
    t[s] = rtems_counter_difference(b, a);
  }

  return prevent_optimization;
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

static void test_by_function_level(int fl, bool dirty)
{
  RTEMS_INTERRUPT_LOCK_DECLARE(, lock)
  rtems_interrupt_lock_context lock_context;
  int s;
  uint64_t min;
  uint64_t q1;
  uint64_t q2;
  uint64_t q3;
  uint64_t max;

  fl += prevent_optimization;

  rtems_interrupt_lock_initialize(&lock, "test");
  rtems_interrupt_lock_acquire(&lock, &lock_context);

  for (s = 0; s < SAMPLES; ++s) {
    call_at_level(fl, fl, s, dirty);
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
    "    <Sample functionNestLevel=\"%i\">\n"
    "      <Min unit=\"ns\">%" PRIu64 "</Min>"
      "<Q1 unit=\"ns\">%" PRIu64 "</Q1>"
      "<Q2 unit=\"ns\">%" PRIu64 "</Q2>"
      "<Q3 unit=\"ns\">%" PRIu64 "</Q3>"
      "<Max unit=\"ns\">%" PRIu64 "</Max>\n"
    "    </Sample>\n",
    fl,
    rtems_counter_ticks_to_nanoseconds(min),
    rtems_counter_ticks_to_nanoseconds(q1),
    rtems_counter_ticks_to_nanoseconds(q2),
    rtems_counter_ticks_to_nanoseconds(q3),
    rtems_counter_ticks_to_nanoseconds(max)
  );
}

static void test(bool dirty, uint32_t load)
{
  int fl;

  printf(
    "  <ContextSwitchTest environment=\"%s\"",
    dirty ? "dirty" : "normal"
  );

  if (load > 0) {
    printf(" load=\"%" PRIu32 "\"", load);
  }

  printf(">\n");

  for (fl = 0; fl < FUNCTION_LEVELS; ++fl) {
    test_by_function_level(fl, dirty);
  }

  printf("  </ContextSwitchTest>\n");
}

static void Init(rtems_task_argument arg)
{
  uint32_t load = 0;

  TEST_BEGIN();

  printf("<Test>\n");

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

  test(false, load);
  test(true, load);

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

    test(true, load);
  }

  printf("</Test>\n");

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
