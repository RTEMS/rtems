/**
 * @file sprmsched01/system.h
 *
 * @brief sprmsched01 example header
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  COPYRIGHT (c) 2016 Kuan-Hsun Chen.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#include <inttypes.h>
#include <rtems.h>

#include <tmacros.h>

/* function prototypes */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task(
  rtems_task_argument argument
);


/*
 *  Keep the names and IDs in global variables so another task can use them.
 */

extern rtems_id   Task_id[ 2 ];         /* array of task ids */
extern rtems_name Task_name[ 2 ];       /* array of task names */
extern uint32_t tick_per_second;        /* time reference */
extern int testnumber;                  /* stop condition */

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MICROSECONDS_PER_TICK     1000   // NB: 10 and lower gives system failure for erc32 simulator
#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_MAXIMUM_SEMAPHORES        1
#define CONFIGURE_MAXIMUM_PRIORITY          15
#define CONFIGURE_EXTRA_TASK_STACKS         (20 * RTEMS_MINIMUM_STACK_SIZE)
#define CONFIGURE_MAXIMUM_PERIODS           3

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* end of include file */
