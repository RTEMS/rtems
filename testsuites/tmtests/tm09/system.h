/*  system.h
 *
 *  This include file is used by all tests in the Time Suite.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <coverhd.h>
#include <tmacros.h>

/* functions */

#define MESSAGE_SIZE (sizeof(long) * 4)

rtems_task Init(
  rtems_task_argument argument
);

#include <timesys.h>

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     1
#define CONFIGURE_TICKS_PER_TIMESLICE        0
#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(OPERATION_COUNT, MESSAGE_SIZE)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* end of include file */
