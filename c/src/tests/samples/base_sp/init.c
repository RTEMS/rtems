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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>

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

  status = rtems_task_create( task_name, 1, RTEMS_MINIMUM_STACK_SIZE,
             RTEMS_INTERRUPT_LEVEL(0), RTEMS_DEFAULT_ATTRIBUTES, &tid );

  status = rtems_task_start( tid, Application_task, ARGUMENT );

  status = rtems_task_delete( RTEMS_SELF );
}
