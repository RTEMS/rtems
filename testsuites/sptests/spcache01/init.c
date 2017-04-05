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

#include <inttypes.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/counter.h>
#include <rtems/score/sysstate.h>

#define TESTS_USE_PRINTF
#include "tmacros.h"

const char rtems_test_name[] = "SPCACHE 1";

#ifdef __or1k__
  #define I() __asm__ volatile ("l.nop")
#else
  #define I() __asm__ volatile ("nop")
#endif

#define I8() I(); I(); I(); I(); I(); I(); I(); I()

#define I64() I8(); I8(); I8(); I8(); I8(); I8(); I8(); I8()

#define I512() I64(); I64(); I64(); I64(); I64(); I64(); I64(); I64()

CPU_STRUCTURE_ALIGNMENT static int data[1024];

static bool do_longjmp;

static jmp_buf instruction_invalidate_return_context;

static void test_data_flush_and_invalidate(void)
{
  if (rtems_cache_get_data_line_size() > 0) {
    RTEMS_INTERRUPT_LOCK_DECLARE(, lock)
    rtems_interrupt_lock_context lock_context;
    volatile int *vdata = &data[0];
    int n = 32;
    int i;
    size_t data_size = n * sizeof(data[0]);
    bool write_through;

    printf("data cache flush and invalidate test\n");

    rtems_interrupt_lock_initialize(&lock, "test");
    rtems_interrupt_lock_acquire(&lock, &lock_context);

    for (i = 0; i < n; ++i) {
      vdata[i] = i;
    }

    rtems_cache_flush_multiple_data_lines(&data[0], data_size);

    for (i = 0; i < n; ++i) {
      rtems_test_assert(vdata[i] == i);
    }

    for (i = 0; i < n; ++i) {
      vdata[i] = ~i;
    }

    rtems_cache_invalidate_multiple_data_lines(&data[0], data_size);

    write_through = vdata[0] == ~0;
    if (write_through) {
      for (i = 0; i < n; ++i) {
        rtems_test_assert(vdata[i] == ~i);
      }
    } else {
      for (i = 0; i < n; ++i) {
        rtems_test_assert(vdata[i] == i);
      }
    }

    for (i = 0; i < n; ++i) {
      vdata[i] = ~i;
    }

    rtems_cache_flush_multiple_data_lines(&data[0], data_size);
    rtems_cache_invalidate_multiple_data_lines(&data[0], data_size);

    for (i = 0; i < n; ++i) {
      rtems_test_assert(vdata[i] == ~i);
    }

    rtems_interrupt_lock_release(&lock, &lock_context);
    rtems_interrupt_lock_destroy(&lock);

    printf(
      "data cache operations by line passed the test (%s cache detected)\n",
      write_through ? "write-through" : "copy-back"
    );
  } else {
    printf(
      "skip data cache flush and invalidate test"
        " due to cache line size of zero\n"
    );
  }

  /* Make sure these are nops */
  rtems_cache_flush_multiple_data_lines(NULL, 0);
  rtems_cache_invalidate_multiple_data_lines(NULL, 0);
}

static uint64_t do_some_work(void)
{
  rtems_counter_ticks a;
  rtems_counter_ticks b;
  rtems_counter_ticks d;

  /* This gives 1024 nop instructions */
  a = rtems_counter_read();
  I512();
  I512();
  b = rtems_counter_read();

  d = rtems_counter_difference(b, a);

  return rtems_counter_ticks_to_nanoseconds(d);
}

static uint64_t load(void)
{
  rtems_counter_ticks a;
  rtems_counter_ticks b;
  rtems_counter_ticks d;
  size_t i;
  volatile int *vdata = &data[0];

  a = rtems_counter_read();
  for (i = 0; i < RTEMS_ARRAY_SIZE(data); ++i) {
    vdata[i];
  }
  b = rtems_counter_read();

  d = rtems_counter_difference(b, a);

  return rtems_counter_ticks_to_nanoseconds(d);
}

static uint64_t store(void)
{
  rtems_counter_ticks a;
  rtems_counter_ticks b;
  rtems_counter_ticks d;
  size_t i;
  volatile int *vdata = &data[0];

  a = rtems_counter_read();
  for (i = 0; i < RTEMS_ARRAY_SIZE(data); ++i) {
    vdata[i] = 0;
  }
  b = rtems_counter_read();

  d = rtems_counter_difference(b, a);

  return rtems_counter_ticks_to_nanoseconds(d);
}

static void test_timing(void)
{
  RTEMS_INTERRUPT_LOCK_DECLARE(, lock)
  rtems_interrupt_lock_context lock_context;
  size_t data_size = sizeof(data);
  uint64_t d[3];
  uint32_t cache_level;
  size_t cache_size;
  bool exception;

  rtems_interrupt_lock_initialize(&lock, "test");

  printf(
    "data cache line size %zi bytes\n"
    "data cache size %zi bytes\n",
    rtems_cache_get_data_line_size(),
    rtems_cache_get_data_cache_size(0)
  );

  cache_level = 1;
  cache_size = rtems_cache_get_data_cache_size(cache_level);
  while (cache_size > 0) {
    printf(
      "data cache level %" PRIu32 " size %zi bytes\n",
      cache_level,
      cache_size
    );
    ++cache_level;
    cache_size = rtems_cache_get_data_cache_size(cache_level);
  }

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = load();
  d[1] = load();
  rtems_cache_flush_entire_data();
  d[2] = load();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "load %zi bytes with flush entire data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with flushed cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = load();
  d[1] = load();
  rtems_cache_flush_multiple_data_lines(&data[0], sizeof(data));
  d[2] = load();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "load %zi bytes with flush multiple data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with flushed cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = load();
  d[1] = load();
  rtems_cache_invalidate_multiple_data_lines(&data[0], sizeof(data));
  d[2] = load();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "load %zi bytes with invalidate multiple data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = store();
  d[1] = store();
  rtems_cache_flush_entire_data();
  d[2] = store();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "store %zi bytes with flush entire data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with flushed cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = store();
  d[1] = store();
  rtems_cache_flush_multiple_data_lines(&data[0], sizeof(data));
  d[2] = store();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "store %zi bytes with flush multiple data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with flushed cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = store();
  d[1] = store();
  rtems_cache_invalidate_multiple_data_lines(&data[0], sizeof(data));
  d[2] = store();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "store %zi bytes with invalidate multiple data\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    data_size,
    d[0],
    d[1],
    d[2]
  );

  printf(
    "instruction cache line size %zi bytes\n"
    "instruction cache size %zi bytes\n",
    rtems_cache_get_instruction_line_size(),
    rtems_cache_get_instruction_cache_size(0)
  );

  cache_level = 1;
  cache_size = rtems_cache_get_instruction_cache_size(cache_level);
  while (cache_size > 0) {
    printf(
      "instruction cache level %" PRIu32 " size %zi bytes\n",
      cache_level,
      cache_size
    );
    ++cache_level;
    cache_size = rtems_cache_get_instruction_cache_size(cache_level);
  }

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = do_some_work();
  d[1] = do_some_work();
  rtems_cache_invalidate_entire_instruction();
  d[2] = do_some_work();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "invalidate entire instruction\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, &lock_context);

  d[0] = do_some_work();
  d[1] = do_some_work();

  do_longjmp = true;

  if (setjmp(instruction_invalidate_return_context) == 0) {
    rtems_cache_invalidate_multiple_instruction_lines(do_some_work, 4096);
    exception = false;
  } else {
    exception = true;
  }

  do_longjmp = false;

  d[2] = do_some_work();

  rtems_interrupt_lock_release(&lock, &lock_context);

  printf(
    "invalidate multiple instruction\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    d[0],
    d[1],
    d[2]
  );

  if (exception) {
    printf(
      "rtems_cache_invalidate_multiple_data_lines() generated an exception\n"
    );
  }

  rtems_interrupt_lock_destroy(&lock);
}

static void test_cache_aligned_alloc(void)
{
  void *p0;
  void *p1;
  size_t cls;

  printf("test rtems_cache_aligned_malloc()\n");

  p0 = rtems_cache_aligned_malloc(1);
  p1 = rtems_cache_aligned_malloc(1);

  rtems_test_assert(p0 != NULL);
  rtems_test_assert(p1 != NULL);

  cls = rtems_cache_get_data_line_size();
  if (cls > 0) {
    size_t m = cls - 1;
    uintptr_t a0 = (uintptr_t) p0;
    uintptr_t a1 = (uintptr_t) p1;

    rtems_test_assert(a1 - a0 > cls);
    rtems_test_assert((a0 & m) == 0);
    rtems_test_assert((a1 & m) == 0);
  }

  free(p0);
  free(p1);
}

#define AREA_SIZE 256

static char cache_coherent_area_0[AREA_SIZE];

static char cache_coherent_area_1[AREA_SIZE];

static char cache_coherent_area_2[AREA_SIZE];

static void add_area(void *begin)
{
  rtems_cache_coherent_add_area(NULL, 0);
  rtems_cache_coherent_add_area(begin, AREA_SIZE);
}

static void test_cache_coherent_alloc(void)
{
  void *p0;
  void *p1;
  System_state_Codes previous_state;

  printf("test cache coherent allocation\n");

  p0 = rtems_cache_coherent_allocate(1, 0, 0);
  rtems_test_assert(p0 != NULL);

  rtems_cache_coherent_free(p0);

  p0 = rtems_cache_coherent_allocate(1, 0, 0);
  rtems_test_assert(p0 != NULL);

  add_area(&cache_coherent_area_0[0]);
  add_area(&cache_coherent_area_1[0]);

  previous_state = _System_state_Get();
  _System_state_Set(previous_state + 1);
  add_area(&cache_coherent_area_2[0]);
  _System_state_Set(previous_state);

  p1 = rtems_cache_coherent_allocate(1, 0, 0);
  rtems_test_assert(p1 != NULL);

  rtems_cache_coherent_free(p0);
  rtems_cache_coherent_free(p1);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_data_flush_and_invalidate();
  test_timing();
  test_cache_aligned_alloc();
  test_cache_coherent_alloc();

  TEST_END();

  rtems_test_exit(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  if (source == RTEMS_FATAL_SOURCE_EXCEPTION && do_longjmp) {
    _ISR_Set_level(0);
    longjmp(instruction_invalidate_return_context, 1);
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
