/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);
 
void put_error(
  rtems_unsigned32  error,
  rtems_status_code expected
);
 
rtems_extension Fatal_extension(
  rtems_unsigned32 source,
  boolean          is_internal,
  rtems_unsigned32 error
);
 
rtems_task Task_1(
  rtems_task_argument argument
);

/* configuration information */

extern rtems_extensions_table initial_extensions;

#define CONFIGURE_INITIAL_EXTENSIONS \
  { \
    NULL,                    /* create  */ \
    NULL,                    /* start   */ \
    NULL,                    /* restart */ \
    NULL,                    /* delete  */ \
    NULL,                    /* switch  */ \
    NULL,                    /* begin   */ \
    NULL,                    /* exitted */ \
    Fatal_extension          /* fatal   */ \
  }

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(0)
#define CONFIGURE_TICKS_PER_TIMESLICE   0

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

TEST_EXTERN rtems_configuration_table        New_Configuration;

extern rtems_extensions_table           Extensions;

/* end of include file */
