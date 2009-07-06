/*  Test fatal error when _Heap_Initialize fails during initialization
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

rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "\n\n*** TEST FATAL 12 ***" );

  puts( "_Heap_Initialize fails during RTEMS initialization" );
  Configuration.work_space_start = (void *)0x03;
  Configuration.work_space_size  = sizeof(void *);
  rtems_initialize_data_structures();
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
