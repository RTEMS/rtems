/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#include <time.h>
#include <rtems.h>
#include <rtems/test.h>
#include <rtems/timespec.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTIMECOUNTER 4";

static void Init(rtems_task_argument arg)
{
  struct timespec real_start;
  struct timespec mono_start;
  struct timespec real_end;
  struct timespec mono_end;
  struct timespec real_elapsed;
  struct timespec mono_elapsed;

  struct timespec min;
  struct timespec max;

  int rv;

  TEST_BEGIN();

  rtems_timespec_set(&min, 1, 980000000);
  rtems_timespec_set(&max, 2,  20000000);

  rv = clock_gettime(CLOCK_REALTIME, &real_start);
  rtems_test_assert(rv == 0);
  rv = clock_gettime(CLOCK_MONOTONIC, &mono_start);
  rtems_test_assert(rv == 0);

  rtems_task_wake_after(2 * rtems_clock_get_ticks_per_second());

  rv = clock_gettime(CLOCK_REALTIME, &real_end);
  rtems_test_assert(rv == 0);
  rv = clock_gettime(CLOCK_MONOTONIC, &mono_end);
  rtems_test_assert(rv == 0);

  rtems_timespec_subtract(&real_start, &real_end, &real_elapsed);
  rtems_timespec_subtract(&mono_start, &mono_end, &mono_elapsed);

  rtems_test_assert(rtems_timespec_greater_than(&real_elapsed, &min));
  rtems_test_assert(rtems_timespec_greater_than(&max, &real_elapsed));
  rtems_test_assert(rtems_timespec_greater_than(&mono_elapsed, &min));
  rtems_test_assert(rtems_timespec_greater_than(&max, &mono_elapsed));

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
