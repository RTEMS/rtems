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

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_REGIONS             1

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 6 ];      /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 6 ];    /* array of task names */
TEST_EXTERN rtems_id   Region_id[ 2 ];    /* array of region ids */
TEST_EXTERN rtems_name Region_name[ 2 ];  /* array of region names */
 
TEST_EXTERN rtems_unsigned8 Area_1[64000]  CPU_STRUCTURE_ALIGNMENT;
 
#define BASE_PRIORITY 140
 
#define Put_address_from_area_1( _to_be_printed ) \
   printf( "0x%08x", ((rtems_unsigned8 *)(_to_be_printed)) - Area_1 )
 
/* end of include file */
