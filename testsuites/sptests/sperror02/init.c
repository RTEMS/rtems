/*
 *  COPYRIGHT (c) 1989-2012.
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

#include "test_support.h"
#include <errno.h>
#include <rtems/error.h>

const char rtems_test_name[] = "SPERROR 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static void fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code error
)
{
  if (
    source == RTEMS_FATAL_SOURCE_EXIT
      && !always_set_to_false
      && error == 1
  ) {
    TEST_END();
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  TEST_BEGIN();

  errno = ENOMEM;
  rtems_error(
    RTEMS_NO_MEMORY | RTEMS_ERROR_ABORT,
    "Dummy: Resources unavailable\n"
  );

  rtems_test_assert(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS \
  { .fatal = fatal_extension }, RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
