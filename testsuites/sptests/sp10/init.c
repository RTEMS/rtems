/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument ignored
)
{
  bool                  is_set;

  puts( "\n\n*** TEST 10 ***" );

  puts( "Init - clear debug level" );
  _Debug_Level = 0;

  puts( "Init - rtems_debug_is_enabled - is 0x1 set? No" );
  is_set = rtems_debug_is_enabled( 0x1 );
  rtems_test_assert(is_set == false);

  puts( "Init - rtems_debug_enable - set 0x1" );
  rtems_debug_enable(0x1);
  rtems_test_assert(_Debug_Level == 0x1);

  puts( "Init - rtems_debug_is_enabled - is 0x1 set? Yes" );
  is_set = rtems_debug_is_enabled( 0x1 );
  rtems_test_assert(is_set == true);

  puts( "Init - rtems_debug_disable - clear 0x1" );
  rtems_debug_disable(0x1);
  rtems_test_assert(_Debug_Level == 0x0);

  puts( "Init - rtems_debug_is_enabled - is 0x1 set? No" );
  is_set = rtems_debug_is_enabled( 0x1 );
  rtems_test_assert(is_set == false);

  puts( "*** END OF TEST 10 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
