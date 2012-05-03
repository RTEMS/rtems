/*  Init
 *
 *  This routine is the initialization task for this test program.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1997
 *  Objective Design Systems Ltd Pty (ODS)
 *  All rights reserved (R) Objective Design Systems Ltd Pty
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "System.h"

// make global so it lasts past the Init task's stack's life time
Task1 task_1;

rtems_task Init(rtems_task_argument )
{
  puts( "\n\n*** RTEMS++ TEST ***" );

  printf( "INIT - Task.create() - " );
  task_1.create("TA1 ", 0, RTEMS_MINIMUM_STACK_SIZE);
  printf("%s\n", task_1.last_status_string());

  printf( "INIT - Task.create() - " );
  task_1.create("TA1 ", 10, RTEMS_MINIMUM_STACK_SIZE * 6);
  printf("%s\n", task_1.last_status_string());

  printf( "INIT - Task.create() - " );
  task_1.create("TA1 ", 10, RTEMS_MINIMUM_STACK_SIZE * 6);
  printf("%s\n", task_1.last_status_string());

  printf( "INIT - Task.restart() - " );
  task_1.restart(0);
  printf("%s\n", task_1.last_status_string());

  printf( "INIT - Task.start(0xDEADDEAD) - " );
  task_1.start(0xDEADDEAD);
  printf("%s\n", task_1.last_status_string());

  printf("INIT - Destroy it's self\n");

  // needs to be in C, no C++ object owns the Init task
  rtems_status_code status  = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}



