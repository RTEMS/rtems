/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/version.h>

#include <tmacros.h>

const char rtems_test_name[] = "VERSION 1";

static rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  printf("Release  : %s\n", rtems_version());
  printf("Major    : %d\n", rtems_version_major());
  printf("Minor    : %d\n", rtems_version_minor());
  printf("Revision : %d\n", rtems_version_revision());
  printf("VC Key   : %s\n", rtems_version_control_key());

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
