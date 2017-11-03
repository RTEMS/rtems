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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/thread.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTLS 3";

static volatile uint32_t read_write_small = 0xdeadbeefUL;

static const volatile uint32_t read_only_small = 0x601dc0feUL;

static void test(void)
{
  Thread_Control *executing = _Thread_Get_executing();

  rtems_test_assert(read_write_small == 0xdeadbeefUL);
  rtems_test_assert(read_only_small == 0x601dc0feUL);

  rtems_test_assert(executing->Start.tls_area == NULL);
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test();

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
