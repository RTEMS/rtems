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

#include <sys/stat.h>
#include <errno.h>

#include <bsp.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPCONFIG 1";

static int counter;

static void checkpoint(int expected_counter)
{
  int current_counter;

  current_counter = counter;
  rtems_test_assert(current_counter == expected_counter);
  counter = current_counter + 1;
}

static rtems_status_code bsp_prerequisite_drivers_init(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  TEST_BEGIN();
  checkpoint(0);
  return RTEMS_SUCCESSFUL;
}

static rtems_status_code app_prerequisite_drivers_init(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  struct stat st;
  int rv;

  checkpoint(1);

  errno = 0;
  rv = stat("/dev/null", &st);
  rtems_test_assert(rv == -1);
  rtems_test_assert(errno == ENOENT);

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code app_extra_drivers_init(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  struct stat st;
  int rv;

  checkpoint(2);

  rv = stat("/dev/null", &st);
  rtems_test_assert(rv == 0);

  return RTEMS_SUCCESSFUL;
}

static void Init(rtems_task_argument arg)
{
  checkpoint(3);
  TEST_END();
  rtems_test_exit(0);
}

#ifdef CONFIGURE_BSP_PREREQUISITE_DRIVERS
#warning "CONFIGURE_BSP_PREREQUISITE_DRIVERS will be #undef for this test"
#undef CONFIGURE_BSP_PREREQUISITE_DRIVERS
#endif

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_BSP_PREREQUISITE_DRIVERS \
  { bsp_prerequisite_drivers_init, NULL, NULL, NULL, NULL, NULL }

#define CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS \
  { app_prerequisite_drivers_init, NULL, NULL, NULL, NULL, NULL }

#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

#define CONFIGURE_APPLICATION_EXTRA_DRIVERS \
  { app_extra_drivers_init, NULL, NULL, NULL, NULL, NULL }

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
