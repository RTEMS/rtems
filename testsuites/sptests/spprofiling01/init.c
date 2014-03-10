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
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/profiling.h>
#include <rtems/bspIo.h>
#include <rtems.h>

#include <stdio.h>

#include "tmacros.h"

static void test(void)
{
  rtems_status_code sc;
  int rv;

  sc = rtems_task_wake_after(3);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  rv = rtems_profiling_report_xml("X", rtems_printf_plugin, NULL, 1, "  ");
  printf("characters produced by rtems_profiling_report_xml(): %i\n", rv);
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST SPPROFILING 1 ***");

  test();

  puts("*** END OF TEST SPPROFILING 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
