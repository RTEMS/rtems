/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

TEST_EXTERN struct counters Count;       /* iteration counters */
TEST_EXTERN struct counters Temporary_count;
extern rtems_task_priority Priorities[ 6 ];

/* end of include file */
