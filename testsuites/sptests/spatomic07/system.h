/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "tmacros.h"
#include "test_support.h"

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Wait_task(
  rtems_task_argument argument
);
/* configuration information */
#define TASK_NUMS 4

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            \
    (2 + TASK_NUMS)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */

TEST_EXTERN volatile bool TaskRan[ TASK_NUMS ];

/*
 *  Handy macros and static inline functions
 */

/* end of include file */
