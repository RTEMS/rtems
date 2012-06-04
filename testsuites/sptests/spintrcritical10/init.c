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

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#include <tmacros.h>
#include <intrcritical.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_id Main_task;

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  rtems_event_set       out;
  int                   resets;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION 10 ***" );

  puts( "Init - Test may not be able to detect case is hit reliably" );
  puts( "Init - Trying to generate timeout while blocking on event" );

  Main_task = rtems_task_self();

  interrupt_critical_section_test_support_initialize( NULL );

  for (resets=0 ; resets< 2 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    sc = rtems_event_receive( 0x01, RTEMS_DEFAULT_OPTIONS, 1, &out );
    fatal_directive_status( sc, RTEMS_TIMEOUT, "event_receive timeout" );
  }

  puts( "*** END OF TEST INTERRUPT CRITICAL SECTION 10 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
