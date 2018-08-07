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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

const char rtems_test_name[] = "SPNSEXT 1";

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  RTEMS_INTERRUPT_LOCK_DECLARE(, lock)
  rtems_interrupt_lock_context lock_context;
  rtems_interval t0 = 0;
  rtems_interval t1 = 0;
  int i = 0;
  int n = 0;
  struct timespec uptime;
  struct timespec new_uptime;

  TEST_BEGIN();

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

  rtems_interrupt_lock_initialize(&lock, "test");
  rtems_interrupt_lock_acquire(&lock, &lock_context);
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
  rtems_interrupt_lock_release(&lock, &lock_context);
  rtems_interrupt_lock_destroy(&lock);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 100000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
