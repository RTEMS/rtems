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
#include "pmacros.h"

#include <sys/stat.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#include "fstest.h"
#include "fstest_support.h"

static rtems_resource_snapshot before_mount;

void
test_initialize_filesystem (void)
{
  int rc = 0;
  rc = mkdir (BASE_FOR_TEST,S_IRWXU|S_IRWXG|S_IRWXO);
  rtems_test_assert (rc == 0);

  rtems_resource_snapshot_take(&before_mount);

  rc = mount (NULL, BASE_FOR_TEST, "imfs", RTEMS_FILESYSTEM_READ_WRITE, NULL);
  rtems_test_assert (rc == 0);
}


void
test_shutdown_filesystem (void)
{
  int rc = 0;
  rc = unmount (BASE_FOR_TEST);
  rtems_test_assert (rc == 0);

  rtems_test_assert(rtems_resource_snapshot_check(&before_mount));
}

/* configuration information */

/* drivers */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

/**
 * Configure base RTEMS resources.
 */

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS                  10
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 40
#define CONFIGURE_INIT_TASK_STACK_SIZE (32 * 1024)
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS  1

#define CONFIGURE_FILESYSTEM_IMFS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
