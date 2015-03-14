/*
 *  COPYRIGHT (c) 1989-2015.
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

const char rtems_test_name[] = "SPNOTEPAD 2";

/*
 * We know this is deprecated and don't want a warning on every BSP built.
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;

  TEST_BEGIN();

  status = rtems_task_set_note( rtems_task_self(), RTEMS_NOTEPAD_4, 32 );
  directive_failed( status, "task_set_node of Self id" );
  printf( "INIT - rtems_task_set_note - set my (id) RTEMS_NOTEPAD_4 " );
  puts  ( "to TA1's priority: 04" );

  status = rtems_task_set_note( RTEMS_SELF, RTEMS_NOTEPAD_4, 32 );
  directive_failed( status, "task_set_node of Self 0" );
  printf( "INIT - rtems_task_set_note - set my (SELF) RTEMS_NOTEPAD_4 " );
  puts  ( "to TA1's priority: 04" );


  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION
#define CONFIGURE_ENABLE_CLASSIC_API_NOTEPADS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* global variables */
