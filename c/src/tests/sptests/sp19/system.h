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
 
rtems_task First_FP_task(
  rtems_task_argument argument
);
 
rtems_task FP_task(
  rtems_task_argument argument
);
 
rtems_task Task_1(
  rtems_task_argument argument
);

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_FLOATING_POINT 

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id Task_id[ 7 ];     /* array of task ids */
TEST_EXTERN rtems_id Task_name[ 7 ];   /* array of task names */

TEST_EXTERN rtems_double FP_factors[ 10 ];  /* FP "uniqueness" factors */
TEST_EXTERN rtems_unsigned32 INTEGER_factors[ 10 ];  /* INT "uniqueness" factors */
/* end of include file */
