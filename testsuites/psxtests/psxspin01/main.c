/**
 *  @file
 *
 *  Simple test program wrapper for Spinlocks
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define CONFIGURE_INIT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <pmacros.h>
 
/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);
void test_main(void);

rtems_task Init(
  rtems_task_argument ignored
)
{
  test_main();
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_MAXIMUM_POSIX_SPINLOCKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)
#define CONFIGURE_INIT_TASK_PRIORITY      2
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT

#define CONFIGURE_EXTRA_TASK_STACKS       RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* end of file */
