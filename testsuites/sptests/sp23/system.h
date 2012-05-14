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
 */

#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_PORTS           1
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 2 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 2 ];   /* array of task names */

TEST_EXTERN rtems_id   Port_id[ 2 ];     /* array of port ids */
TEST_EXTERN rtems_name Port_name[ 2 ];   /* array of port names */

#define Internal_port_area ((uint8_t   *) 0x00001000)
#define External_port_area ((uint8_t   *) 0x00002000)

#define Below_port_area    ((uint8_t   *) 0x00000500)
#define Above_port_area    ((uint8_t   *) 0x00003000)

/* end of include file */
