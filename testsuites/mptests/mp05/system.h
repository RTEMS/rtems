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

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_asr Process_asr( rtems_signal_set );

/* configuration information */
 
#define CONFIGURE_MPTEST
 
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
 
#define CONFIGURE_MAXIMUM_TIMERS              1

#include <confdefs.h>

/* variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];     /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];   /* array of task names */

TEST_EXTERN rtems_id   Timer_id[ 2 ];    /* array of timer ids */
TEST_EXTERN rtems_name Timer_name[ 2 ];  /* array of timer names */
 
TEST_EXTERN volatile rtems_boolean Stop_Test;
 
TEST_EXTERN rtems_unsigned32          remote_node;
TEST_EXTERN rtems_id                  remote_tid;
TEST_EXTERN rtems_signal_set          remote_signal;
TEST_EXTERN rtems_signal_set          expected_signal;
TEST_EXTERN volatile rtems_unsigned32 signal_count;
TEST_EXTERN volatile rtems_unsigned32 signal_caught;
 
/* end of include file */
