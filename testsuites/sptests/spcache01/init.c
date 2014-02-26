/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <stdio.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/counter.h>

#define TESTS_USE_PRINTF
#include "tmacros.h"

#define I() __asm__ volatile ("nop")

#define I8() I(); I(); I(); I(); I(); I(); I(); I()

#define I64() I8(); I8(); I8(); I8(); I8(); I8(); I8(); I8()

#define I512() I64(); I64(); I64(); I64(); I64(); I64(); I64(); I64()

CPU_STRUCTURE_ALIGNMENT static int data[1024];

static void test_data_flush_and_invalidate(void)
{
  if (rtems_cache_get_data_line_size() > 0) {
    rtems_interrupt_level level;
    rtems_interrupt_lock lock = RTEMS_INTERRUPT_LOCK_INITIALIZER;
    volatile int *vdata = &data[0];
    int n = 32;
    int i;
    size_t data_size = n * sizeof(data[0]);
    bool write_through;

    printf("data cache flush and invalidate test\n");

    rtems_interrupt_lock_acquire(&lock, level);

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

    rtems_interrupt_lock_release(&lock, level);

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
  rtems_interrupt_level level;
  rtems_interrupt_lock lock = RTEMS_INTERRUPT_LOCK_INITIALIZER;
  size_t data_size = sizeof(data);
  uint64_t d[3];

  printf(
    "data cache line size %i bytes\n",
    rtems_cache_get_data_line_size()
  );

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = load();
  d[1] = load();
  rtems_cache_flush_entire_data();
  d[2] = load();

  rtems_interrupt_lock_release(&lock, level);

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

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = load();
  d[1] = load();
  rtems_cache_flush_multiple_data_lines(&data[0], sizeof(data));
  d[2] = load();

  rtems_interrupt_lock_release(&lock, level);

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

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = load();
  d[1] = load();
  rtems_cache_invalidate_multiple_data_lines(&data[0], sizeof(data));
  d[2] = load();

  rtems_interrupt_lock_release(&lock, level);

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

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = store();
  d[1] = store();
  rtems_cache_flush_entire_data();
  d[2] = store();

  rtems_interrupt_lock_release(&lock, level);

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

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = store();
  d[1] = store();
  rtems_cache_flush_multiple_data_lines(&data[0], sizeof(data));
  d[2] = store();

  rtems_interrupt_lock_release(&lock, level);

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

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = store();
  d[1] = store();
  rtems_cache_invalidate_multiple_data_lines(&data[0], sizeof(data));
  d[2] = store();

  rtems_interrupt_lock_release(&lock, level);

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
    "instruction cache line size %i bytes\n",
    rtems_cache_get_instruction_line_size()
  );

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = do_some_work();
  d[1] = do_some_work();
  rtems_cache_invalidate_entire_instruction();
  d[2] = do_some_work();

  rtems_interrupt_lock_release(&lock, level);

  printf(
    "invalidate entire instruction\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    d[0],
    d[1],
    d[2]
  );

  rtems_interrupt_lock_acquire(&lock, level);

  d[0] = do_some_work();
  d[1] = do_some_work();
  rtems_cache_invalidate_multiple_instruction_lines(do_some_work, 4096);
  d[2] = do_some_work();

  rtems_interrupt_lock_release(&lock, level);

  printf(
    "invalidate multiple instruction\n"
    "  duration with normal cache %" PRIu64 " ns\n"
    "  duration with warm cache %" PRIu64 " ns\n"
    "  duration with invalidated cache %" PRIu64 " ns\n",
    d[0],
    d[1],
    d[2]
  );
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SPCACHE 1 ***");

  test_data_flush_and_invalidate();
  test_timing();

  puts("*** END OF TEST SPCACHE 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
