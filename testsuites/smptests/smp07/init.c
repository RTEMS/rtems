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

volatile bool TaskRan = false;
volatile bool TSRFired = false;
rtems_id      Semaphore; 

rtems_task Test_task(
  rtems_task_argument argument
)
{
  int               cpu_num;
  rtems_status_code sc;
  char              name[5];
  char             *p;

  /* Get the task name */
  p = rtems_object_get_name( RTEMS_SELF, 5, name );
  rtems_test_assert( p != NULL );

   /* Get the CPU Number */
  cpu_num = bsp_smp_processor_id();

  /* Print that the task is up and running. */
  locked_printf(" CPU %d runnng Task %s and blocking\n", cpu_num, name);

  sc = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  directive_failed( sc,"obtain in test task");

  if ( !TSRFired )
    locked_printf( "*** ERROR TSR DID NOT FIRE BUT TEST TASK AWAKE***" );

  TaskRan = true;

  /* Print that the task is up and running. */
  locked_printf(
    " CPU %d running Task %s after semaphore release\n", 
    cpu_num, 
    name
  );

  (void) rtems_task_delete( RTEMS_SELF );
}


rtems_timer_service_routine TimerMethod(
  rtems_id  timer,
  void     *arg
)
{
  /*
   * Set flag and release the semaphore, allowing the blocked tasks to start.
   */
  TSRFired = true;

  rtems_semaphore_release( Semaphore );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  int                cpu_num;
  rtems_id           id;
  rtems_status_code  status;
  rtems_interval     per_second;
  rtems_interval     then;
  rtems_id           Timer;

  locked_print_initialize();
  locked_printf( "\n\n*** TEST SMP07 ***\n" );

  /* Create/verify semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create" );

  /* Lock semaphore */
  status = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, 0);
  directive_failed( status,"rtems_semaphore_obtain of SEM1\n");

  /* Create and Start test task. */
  status = rtems_task_create(
    rtems_build_name( 'T', 'A', '1', ' ' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  directive_failed( status, "task create" );

  cpu_num = bsp_smp_processor_id();
  locked_printf(" CPU %d start task TA1\n", cpu_num );
  status = rtems_task_start( id, Test_task, 1 );
  directive_failed( status, "task start" );

  /* Create and start TSR */
  locked_printf(" CPU %d create and start timer\n", cpu_num );
  status = rtems_timer_create( rtems_build_name( 'T', 'M', 'R', '1' ), &Timer);
  directive_failed( status, "rtems_timer_create" );

  per_second = rtems_clock_get_ticks_per_second();
  status = rtems_timer_fire_after( Timer, 2 * per_second, TimerMethod, NULL );
  directive_failed( status, "rtems_timer_fire_after");

  /*
   *  Wait long enough that TSR should have fired.
   *
   *  Spin so CPU 0 is consumed.  This forces task to run on CPU 1.
   */
  then = rtems_clock_get_ticks_since_boot() + 4 * per_second;
  while (1) {
    if ( rtems_clock_get_ticks_since_boot() > then )
      break;
    if ( TSRFired && TaskRan )
      break;
  };
  
  /* Validate the timer fired and that the task ran */
  if ( !TSRFired )
    locked_printf( "*** ERROR TSR DID NOT FIRE ***" );

  if ( !TaskRan ) {
    locked_printf( "*** ERROR TASK DID NOT RUN ***" );
    rtems_test_exit(0);
  }

  /* End the program */
  locked_printf( "*** END OF TEST SMP07 ***\n" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_SMP_APPLICATION
#define CONFIGURE_SMP_MAXIMUM_PROCESSORS   2
#define CONFIGURE_MAXIMUM_TIMERS           1

#define CONFIGURE_MAXIMUM_TASKS            2
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_SEMAPHORES       1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
