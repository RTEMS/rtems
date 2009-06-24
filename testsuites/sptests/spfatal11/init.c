/*  Test __assert_func with NULL function name
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#undef NDEBUG
#include <assert.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST FATAL 11 ***" );

  puts( "asserting with non-NULL strings...\n" );
  __assert_func( __FILE__, __LINE__, NULL, "forced" );

}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
