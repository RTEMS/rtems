/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <tmacros.h>

/* types */

struct counters {
  rtems_unsigned32 count[6];
};

/* functions */

rtems_task Init(
  rtems_task_argument argument
);
 
rtems_task Task_1_through_5(
  rtems_task_argument argument
);
 
void Get_all_counters( void );
 
/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INIT_TASK_PRIORITY          10
#define CONFIGURE_INIT_TASK_INITIAL_MODES     RTEMS_DEFAULT_MODES
#define CONFIGURE_MAXIMUM_PERIODS             10

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 6 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 6 ];   /* array of task names */

/* end of include file */
