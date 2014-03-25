/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"

const char rtems_test_name[] = "SPNOTEPAD 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  uint32_t          note;

  TEST_BEGIN();

  puts( "Init - rtems_task_get_note - RTEMS_NOT_CONFIGURED" );
  status = rtems_task_get_note( rtems_task_self(), 0, &note );
  fatal_directive_status(
    status,
    RTEMS_NOT_CONFIGURED,
    "rtems_task_get_note not configured"
  );

  puts( "Init - rtems_task_set_note - RTEMS_NOT_CONFIGURED" );
  status = rtems_task_set_note( rtems_task_self(), 0, 1 );
  fatal_directive_status(
    status,
    RTEMS_NOT_CONFIGURED,
    "rtems_task_set_note not configured"
  );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_DISABLE_CLASSIC_API_NOTEPADS
#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* global variables */
