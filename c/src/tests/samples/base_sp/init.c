/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the application, the current time might be
 *  set by this task.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#define TEST_INIT
#include "system.h"

#define ARGUMENT 0

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_name        task_name;
  rtems_id          tid;
  rtems_status_code status;

  printf( "\n\n*** SAMPLE SINGLE PROCESSOR APPLICATION ***\n" );
  printf( "Creating and starting an application task\n" );

  task_name = rtems_build_name( 'T', 'A', '1', ' ' );

  status = rtems_task_create( task_name, 1, 1024,
             RTEMS_INTERRUPT_LEVEL(0), RTEMS_DEFAULT_ATTRIBUTES, &tid );

  status = rtems_task_start( tid, Application_task, ARGUMENT );

  status = rtems_task_delete( RTEMS_SELF );
}
