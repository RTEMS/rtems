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

#include "tmacros.h"

#include <rtems.h>

const char rtems_test_name[] = "SPFATAL 31";

#if SPARC_HAS_FPU == 1
#define EXPECT_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
#endif

static volatile double f = 1.0;

static void timer(rtems_id id, void *arg)
{
  f *= 123.456;
}

static void Init(rtems_task_argument arg)
{
  rtems_status_code sc;
  rtems_id id;

  TEST_BEGIN();

  sc = rtems_timer_create(rtems_build_name('T', 'I', 'M', 'E'), &id);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_timer_fire_after(id, 1, timer, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_task_wake_after(2);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

#ifdef EXPECT_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
  rtems_test_assert(0);
#else
  TEST_END();
  rtems_test_exit(0);
#endif
}

#ifdef EXPECT_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  if (
    source == INTERNAL_ERROR_CORE
      && !always_set_to_false
      && code == INTERNAL_ERROR_ILLEGAL_USE_OF_FLOATING_POINT_UNIT
  ) {
    TEST_END();
  }
}

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#else /* EXPECT_ILLEGAL_USE_OF_FLOATING_POINT_UNIT */

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#endif /* EXPECT_ILLEGAL_USE_OF_FLOATING_POINT_UNIT */

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1
#define CONFIGURE_MAXIMUM_TIMERS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
