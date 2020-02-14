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
#include <tmacros.h>

#include <sys/stat.h>

#include "fstest.h"
#include "fstest_support.h"

void
test_initialize_filesystem (void)
{
  int rc = 0;
  rc = mkdir (BASE_FOR_TEST,S_IRWXU|S_IRWXG|S_IRWXO);
  rtems_test_assert (rc == 0);
}

void
test_shutdown_filesystem (void)
{
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS                  10
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 40
#define CONFIGURE_INIT_TASK_STACK_SIZE (16 * 1024)
#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
