/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <stdio.h>

void Loop() {
  volatile int i;

  for (i=0; i<300000; i++);
}

void PrintTaskInfo(
  const char *task_name
)
{
  int               cpu_num;
  rtems_status_code sc;

  cpu_num = bsp_smp_processor_id();

  locked_printf("  CPU %d running task %s\n", cpu_num, task_name );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int               i;
  char              ch;
  rtems_id          id;
  rtems_status_code status;

  Loop();
  locked_print_initialize();

  locked_printf( "\n\n***  SMP03 TEST ***" );


  /* Show that the init task is running on this cpu */
  PrintTaskInfo( "Init" );

  /* for each remaining cpu create and start a task */
  for ( i=1; i < CONFIGURE_SMP_MAXIMUM_PROCESSORS; i++ ){

    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      CONFIGURE_INIT_TASK_PRIORITY + (2*i),
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_PREEMPT,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    status = rtems_task_start( id, Test_task, i );

    Loop();
  }

  /* Create/Start an aditional task with the highest priority */
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', ch, ' ' ),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  TestFinished = false;
  status = rtems_task_start( id, Test_task, CONFIGURE_SMP_MAXIMUM_PROCESSORS );

  /* Wait on the last task to run */
  while(!TestFinished)
    ;

  /* End the test */
  locked_printf( "*** END OF TEST SMP03 ***" );
  rtems_test_exit( 0 );    
}
