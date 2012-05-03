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

#define PT_NAME       rtems_build_name( 'P', 'A', 'R', '\0' )

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_MP_APPLICATION

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS     1
#define CONFIGURE_MP_MAXIMUM_PROXIES            0

#define CONFIGURE_MAXIMUM_TASKS               1
#if ( NODE_NUMBER == 1 )
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#elif ( NODE_NUMBER == 2 )
#define CONFIGURE_MAXIMUM_PARTITIONS          2
#endif

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];   /* array of task names */

TEST_EXTERN rtems_id   Partition_id[ 2 ];   /* array of partition ids */
TEST_EXTERN rtems_name Partition_name[ 2 ]; /* array of partition names */

/* end of include file */
