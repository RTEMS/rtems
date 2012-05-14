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

rtems_task Init(
  rtems_task_argument argument
);

#include <timesys.h>

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              (2 + OPERATION_COUNT)
#define CONFIGURE_MAXIMUM_TIMERS             OPERATION_COUNT
#define CONFIGURE_MAXIMUM_SEMAPHORES         OPERATION_COUNT
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     OPERATION_COUNT
#define CONFIGURE_MAXIMUM_PARTITIONS         OPERATION_COUNT
#define CONFIGURE_MAXIMUM_REGIONS            OPERATION_COUNT
#define CONFIGURE_MAXIMUM_PORTS              OPERATION_COUNT
#define CONFIGURE_MAXIMUM_PERIODS            OPERATION_COUNT

#define CONFIGURE_TICKS_PER_TIMESLICE        0

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* end of include file */
