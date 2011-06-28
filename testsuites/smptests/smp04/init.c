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

  for (i=0; i<500000; i++);
}

void PrintTaskInfo(
  const char *task_name
)
{
  int               cpu_num;

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
  bool              allDone;

  Loop();
  locked_print_initialize();
  locked_printf( "\n\n***  SMP04 TEST ***\n" );

  PrintTaskInfo( "Init" );

  TaskRan[0] = true;
  for ( i=1; i <= rtems_smp_get_number_of_processors() ; i++ )
    TaskRan[i] = false;
  
  for ( i=1; i < rtems_smp_get_number_of_processors() ; i++ ){

    /* Create and start tasks for each CPU */
    ch = '0' + i;
    locked_printf(
      "Create a TA%c a %s task\n",
      ch, 
      ((i%2) ? "RTEMS_PREEMPT" : "RTEMS_NO_PREEMPT" ) 
    );

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      CONFIGURE_INIT_TASK_PRIORITY +
        (2*rtems_smp_get_number_of_processors()) - (2*i),
      RTEMS_MINIMUM_STACK_SIZE,
      ((i%2) ? RTEMS_PREEMPT : RTEMS_NO_PREEMPT),
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
  }

  for ( i=1; i < rtems_smp_get_number_of_processors() ; i++ ){
    status = rtems_task_start( id, Test_task, i );

    /* Force a wait on the task to run in order to synchronize on
     * simulated systems.
     */   
    while (TaskRan[i] == false)
      ;
  }

  ch = '0' + rtems_smp_get_number_of_processors() ;

  status = rtems_task_create(
    rtems_build_name( 'T', 'A', ch, ' ' ),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  status = rtems_task_start(
    id,
    Test_task,
    rtems_smp_get_number_of_processors()
  );
  
  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=1; i<=rtems_smp_get_number_of_processors() ; i++ ) {
      if (TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      Loop();
      locked_printf( "*** END OF TEST SMP04 ***" );
      rtems_test_exit( 0 );
    }
  }
}
