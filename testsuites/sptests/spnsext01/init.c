/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
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

#include <tmacros.h>

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_interrupt_level level;
  rtems_interval t0 = 0;
  rtems_interval t1 = 0;
  int i = 0;
  int n = 0;
  struct timespec uptime;
  struct timespec new_uptime;

  puts("\n\n*** TEST NANO SECONDS EXTENSION 1 ***");

  /* Align with clock tick */
  t0 = rtems_clock_get_ticks_since_boot();
  while ((t1 = rtems_clock_get_ticks_since_boot()) == t0) {
    /* Do nothing */
  }

  t0 = t1;
  sc = rtems_clock_get_uptime(&uptime);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  while ((t1 = rtems_clock_get_ticks_since_boot()) == t0) {
    ++n;
    sc = rtems_clock_get_uptime(&new_uptime);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(!_Timespec_Less_than(&new_uptime, &uptime));
    uptime = new_uptime;
  }

  n = (3 * n) / 2;

  rtems_interrupt_disable(level);
  sc = rtems_clock_get_uptime(&uptime);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  for (i = 0; i < n; ++i) {
    /* Preserve execution time of previous loop */
    rtems_clock_get_ticks_since_boot();

    sc = rtems_clock_get_uptime(&new_uptime);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
    rtems_test_assert(!_Timespec_Less_than(&new_uptime, &uptime));
    uptime = new_uptime;
  }
  rtems_interrupt_enable(level);

  puts("*** END OF TEST NANO SECONDS EXTENSION 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
