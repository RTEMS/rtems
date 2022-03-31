/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
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

#include <tmacros.h>  /* includes bsp.h, stdio, etc... */

const char rtems_test_name[] = "SP 32";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
    rtems_task_argument ignored
) {
  rtems_status_code  status;
  rtems_interval     timestamps[6],
                     wantintervals[5] = { 1, 50, 200, 25, 3 };
  rtems_name         period_name = rtems_build_name('P','E','R','a');
  rtems_id           period_id;
  int                loopy;

  TEST_BEGIN();

  /* create period */
  status = rtems_rate_monotonic_create(
      period_name,
      &period_id
  );
  directive_failed(status, "rate_monotonic_create");

  /* start period with initial value */
  status = rtems_rate_monotonic_period( period_id, wantintervals[0] );
  directive_failed(status, "rate_monotonic_period #1");

  /* get our first timestamp */
  timestamps[0] = rtems_clock_get_ticks_since_boot();

  /* loop through and gather more timestamps */
  for (loopy = 1; loopy < 5; loopy++) {

    status = rtems_rate_monotonic_period( period_id, wantintervals[loopy] );
    directive_failed(status, "rate_monotonic_period #2");

    timestamps[loopy] = rtems_clock_get_ticks_since_boot();
  }

  /* block one last time */
  status = rtems_rate_monotonic_period( period_id, 1 );
  directive_failed(status, "rate_monotonic_period #3");

  /* get one last timestamp */
  timestamps[loopy] = rtems_clock_get_ticks_since_boot();

  /* cancel the period */
  status = rtems_rate_monotonic_cancel(period_id);
  directive_failed(status, "rate_monotonic_cancel");

  /* delete it */
  status = rtems_rate_monotonic_delete(period_id);
  directive_failed(status, "rate_monotonic_delete");

  /* tabulate and print results */
  for (loopy = 0; loopy < 5; loopy++) {
    printf(
      "period %d: measured %" PRIdrtems_interval " tick(s), wanted %"
        PRIdrtems_interval "\n",
      loopy,
      timestamps[loopy+1] - timestamps[loopy],
      wantintervals[loopy]
    );
  }

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS        1
#define CONFIGURE_MAXIMUM_PERIODS      1

#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

