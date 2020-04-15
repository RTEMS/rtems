/*
 * Copyright (c) 2013, 2018 embedded brains GmbH.  All rights reserved.
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

#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>

#include "tmacros.h"

const char rtems_test_name[] = "BLOCK 17";

static void test(void)
{
  rtems_status_code sc;

  sc = rtems_bdbuf_init();
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);

  sc = rtems_blkdev_create(NULL, 0, 0, NULL, NULL);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
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

#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS (64UL * 1024UL)

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
