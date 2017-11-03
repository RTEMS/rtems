/*
 *  This include file contains information that is included in every
 *  function in the test set.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               1
#define CONFIGURE_TICKS_PER_TIMESLICE       100

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

/* end of include file */
