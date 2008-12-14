/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
 *
 *  COPYRIGHT (c) 1989-2008.
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

rtems_task Task_1(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS               2
#define CONFIGURE_MAXIMUM_REGIONS             1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 6 ];      /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 6 ];    /* array of task names */
TEST_EXTERN rtems_id   Region_id[ 2 ];    /* array of region ids */
TEST_EXTERN rtems_name Region_name[ 2 ];  /* array of region names */

/* test will fail... segment sizes need to be reworked for <=32K buffer */
#if defined(__m32c__)
#warning "Address size type of regions"
TEST_EXTERN uint8_t   Area_1[32000]  CPU_STRUCTURE_ALIGNMENT;
#else
TEST_EXTERN uint8_t   Area_1[64000]  CPU_STRUCTURE_ALIGNMENT;
#endif

#define BASE_PRIORITY ((RTEMS_MAXIMUM_PRIORITY / 2) + 1)

#define Put_address_from_area_1( _to_be_printed ) \
   printf( "0x%08lx", \
     (unsigned long) ((uint8_t   *)(_to_be_printed) - Area_1 ) )

/* end of include file */
