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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/telnetd.h>

#include <tmacros.h>

const char rtems_test_name[] = "TELNETD 1";

struct rtems_bsdnet_config rtems_bsdnet_config;

static void command(char *device_name, void *arg)
{
}

static void test_command_null(void)
{
  static const rtems_telnetd_config_table config = {
    .command = NULL
  };
  rtems_status_code sc;

  sc = rtems_telnetd_start(&config);
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

static void test_cannot_start_server_task(void)
{
  static const rtems_telnetd_config_table config = {
    .command = command,
    .priority = UINT32_MAX
  };
  rtems_status_code sc;

  sc = rtems_telnetd_start(&config);
  rtems_test_assert(sc == RTEMS_UNSATISFIED);
}

static void test_successful_start(void)
{
  static const rtems_telnetd_config_table config = {
    .command = command,
    .stack_size = RTEMS_MINIMUM_STACK_SIZE
  };
  rtems_status_code sc;

  sc = rtems_telnetd_start(&config);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_already_started(void)
{
  static const rtems_telnetd_config_table config = {
    .command = command
  };
  rtems_status_code sc;

  sc = rtems_telnetd_start(&config);
  rtems_test_assert(sc == RTEMS_RESOURCE_IN_USE);
}

static rtems_task Init(rtems_task_argument argument)
{
  int rv;

  TEST_BEGIN();

  rv = rtems_bsdnet_initialize_network();
  rtems_test_assert(rv == 0);

  test_command_null();
  test_cannot_start_server_task();
  test_successful_start();
  test_already_started();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS (3 + 1 + 5 * 4)

#define CONFIGURE_MAXIMUM_TASKS 8

#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#include <rtems/confdefs.h>
