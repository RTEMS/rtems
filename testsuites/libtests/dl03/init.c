/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "dl-cache.h"
#include "dl-bit-alloc.h"

const char rtems_test_name[] = "libdl (RTL) 3";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

static int test(void)
{
  int ret;
  ret = dl_cache_test();
  if (ret)
    rtems_test_exit(ret);
  ret = dl_bit_alloc_test();
  if (ret)
    rtems_test_exit(ret);
  return 0;
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

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_STACK_SIZE (8U * 1024U)

#define CONFIGURE_INIT_TASK_ATTRIBUTES   (RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT)

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
