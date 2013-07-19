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

#include <tmacros.h>
#include "test_support.h"

volatile bool Ran;

static void success(void)
{
  locked_printf( "*** END OF TEST SMP06 ***\n" );
  rtems_test_exit( 0 );
}

rtems_task Test_task(
  rtems_task_argument do_exit
)
{
  uint32_t          cpu_num;
  char              name[5];
  char             *p;

  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

  cpu_num = rtems_smp_get_current_processor();
  locked_printf(" CPU %" PRIu32 " running Task %s\n", cpu_num, name);

  Ran = true;

  if ( do_exit ) {
    success();
  }
  while(1)
    ;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  uint32_t           cpu_num;
  rtems_id           id;
  rtems_status_code  status;

  locked_print_initialize();
  locked_printf( "\n\n*** TEST SMP06 ***\n" );

  if ( rtems_smp_get_processor_count() == 1 ) {
    success();
  }

  locked_printf( "rtems_clock_tick - so this task has run longer\n" );
  _Thread_Disable_dispatch();
  status = rtems_clock_tick();
  _Thread_Enable_dispatch();
  directive_failed( status, "clock tick" );

  cpu_num = rtems_smp_get_current_processor();

  /*
   * Create a task at equal priority.
   */
  Ran = false;
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "task create" );

  locked_printf(" CPU %" PRIu32 " start task TA1\n", cpu_num );

  status = rtems_task_start( id, Test_task, 0 );
  directive_failed( status, "task start" );

  while ( Ran == false )
    ;

  /*
   * Create a task at greater priority.
   */
  Ran = false;
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '2', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "task create" );

  cpu_num = rtems_smp_get_current_processor();
  locked_printf(" CPU %" PRIu32 " start task TA2\n", cpu_num );

  status = rtems_task_start( id, Test_task, 1 );
  directive_failed( status, "task start" );

  while ( 1 )
    ;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_SMP_APPLICATION
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS  2

#define CONFIGURE_MAXIMUM_TASKS           4

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT_TASK_PRIORITY      2
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
