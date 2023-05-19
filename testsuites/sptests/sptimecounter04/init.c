/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <time.h>
#include <rtems.h>
#include <rtems/test-info.h>
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
