/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#include <stdlib.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPGLOBALCON 2";

static int i;

static __attribute__((__constructor__(1000))) void c0(void)
{
  rtems_test_assert(i == 0);
  ++i;
}

static __attribute__((__constructor__(1001))) void c1(void)
{
  rtems_test_assert(i == 1);
  ++i;
}

static __attribute__((__constructor__(1002))) void c2(void)
{
  rtems_test_assert(i == 2);
  ++i;
}

static __attribute__((__constructor__)) void c(void)
{
  rtems_test_assert(i == 3);
  ++i;
}

static __attribute__((__destructor__(1000))) void d0(void)
{
  rtems_test_assert(i == 8);
  ++i;
}

static __attribute__((__destructor__(1001))) void d1(void)
{
  rtems_test_assert(i == 7);
  ++i;
}

static __attribute__((__destructor__(1002))) void d2(void)
{
  rtems_test_assert(i == 6);
  ++i;
}

static __attribute__((__destructor__)) void d(void)
{
  rtems_test_assert(i == 5);
  ++i;
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();
  rtems_test_assert(i == 4);
  ++i;
  exit(0);
}

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  if (
    source == RTEMS_FATAL_SOURCE_EXIT
      && !always_set_to_false
      && error == 0
      && i == 9
  ) {
    TEST_END();
  }
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, \
  RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
