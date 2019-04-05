/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <stdio.h>
#include <inttypes.h>

const char rtems_test_name[] = "SMP 3";

static void success(void)
{
  TEST_END();
  rtems_test_exit( 0 );
}

void Loop() {
  volatile int i;

  for (i=0; i<300000; i++);
}

void PrintTaskInfo(
  const char *task_name
)
{
  uint32_t cpu_num;

  cpu_num = rtems_scheduler_get_processor();

  locked_printf("  CPU %" PRIu32 " running task %s\n", cpu_num, task_name );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t          i;
  char              ch = '0';
  rtems_id          id;
  rtems_status_code status;
  uint32_t          cpu_max;

  Loop();

  TEST_BEGIN();

  locked_print_initialize();
  cpu_max = rtems_scheduler_get_processor_maximum();

  if ( cpu_max == 1 ) {
    success();
  }

  /* Initialize the TaskRan array */
  TaskRan[0] = true;
  for ( i=1; i<cpu_max ; i++ ) {
    TaskRan[i] = false;
  }

  /* Show that the init task is running on this cpu */
  PrintTaskInfo( "Init" );

  /* for each remaining cpu create and start a task */
  for ( i=1; i < cpu_max; i++ ){

    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      CONFIGURE_INIT_TASK_PRIORITY + (2*i),
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_PREEMPT,
      RTEMS_FLOATING_POINT,
      &id
    );
    directive_failed( status, "rtems_task_create" );
    status = rtems_task_start( id, Test_task, i );
    directive_failed( status, "rtems_task_start" );
    
    /* Allow task to start before starting next task.
     * This is necessary on some simulators.
     */ 
    while (TaskRan[i] == false)
      ;
  }

  /* Create/Start an aditional task with the highest priority */
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', ch + 1, ' ' ),
    3,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_FLOATING_POINT,
    &id
  );
  directive_failed( status, "rtems_task_create" );
  status = rtems_task_start(id,Test_task,cpu_max);
  directive_failed( status, "rtems_task_start" );

  /* Wait on all tasks to run */
  while (1) {
    TestFinished = true;
    for ( i=1; i < (cpu_max+1) ; i++ ) {
      if (TaskRan[i] == false)
        TestFinished = false;
    }
    if (TestFinished) {
      success();
    }
  }

  rtems_test_exit( 0 );    
}
