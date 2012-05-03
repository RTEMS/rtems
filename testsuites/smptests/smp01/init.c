/*
 *  COPYRIGHT (c) 1989-2011.
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
#include "system.h"

void Loop() {
  volatile int i;

  for (i=0; i<300000; i++);
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int                i;
  char               ch;
  int                cpu_num;
  rtems_id           id;
  rtems_status_code  status;
  bool               allDone;

  /* XXX - Delay a bit to allow debug messages from
   * startup to print.  This may need to go away when
   * debug messages go away.
   */ 
  Loop();
  locked_print_initialize();

  /* Put start of test message */
  locked_printf( "\n\n***  SMP01 TEST ***\n" );

  /* Initialize the TaskRan array */
  for ( i=0; i<rtems_smp_get_number_of_processors() ; i++ ) {
    TaskRan[i] = false;
  }

  /* Create and start tasks for each processor */
  for ( i=1; i< rtems_smp_get_number_of_processors() ; i++ ) {
    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "task create" );

    cpu_num = bsp_smp_processor_id();
    locked_printf(" CPU %d start task TA%c\n", cpu_num, ch);
    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );

    Loop();
  }
  
  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=1; i<rtems_smp_get_number_of_processors() ; i++ ) {
      if (TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      locked_printf( "*** END OF TEST SMP01 ***\n" );
      rtems_test_exit( 0 );
    }
  }

}
