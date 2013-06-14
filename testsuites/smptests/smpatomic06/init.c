/*
 * Copyright (c) 2012 Deng Hengyi.
 *
 *  This test case is to test atomic or operation.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

rtems_task Init(
  rtems_task_argument argument
)
{
  int                i;
  char               ch;
  rtems_id           id;
  rtems_status_code  status;
  bool               allDone;

  /* XXX - Delay a bit to allow debug messages from
   * startup to print.  This may need to go away when
   * debug messages go away.
   */
  locked_print_initialize();

  /* Put start of test message */
  locked_printf( "\n\n***  SMPatomic06 TEST ***\n" );

  /* Initialize the TaskRan array */
  for ( i=0; i<rtems_smp_get_processor_count() ; i++ ) {
    TaskRan[i] = false;
  }

  /* Create and start tasks for each processor */
  for ( i=1; i< rtems_smp_get_processor_count() ; i++ ) {
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

    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, "task start" );
  }

  /* Wait on the all tasks to run */
  while (1) {
    allDone = true;
    for ( i=1; i<rtems_smp_get_processor_count() ; i++ ) {
      if (TaskRan[i] == false)
        allDone = false;
    }
    if (allDone) {
      locked_printf( "\n\n*** END OF TEST SMPatomic06 ***\n" );
      rtems_test_exit( 0 );
    }
  }
}
