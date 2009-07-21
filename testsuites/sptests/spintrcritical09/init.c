/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#include <tmacros.h>
#include <intrcritical.h>

rtems_id Main_task;
rtems_id Flusher_id;
rtems_id Semaphore;
volatile bool case_hit;

Thread_blocking_operation_States getState(void)
{
  Objects_Locations  location;
  Semaphore_Control *sem;

  sem = (Semaphore_Control *)_Objects_Get(
    &_Semaphore_Information, Semaphore, &location ); 
  if ( location != OBJECTS_LOCAL ) {
    puts( "Bad object lookup" );
    rtems_test_exit(0);
  }
  _Thread_Unnest_dispatch();

  return sem->Core_control.semaphore.Wait_queue.sync_state;
}

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  if ( getState() == THREAD_BLOCKING_OPERATION_TIMEOUT ) {
    case_hit = true;
  }
}

rtems_task Flusher(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  while (1) {
    sc = rtems_semaphore_flush( Semaphore );
    directive_failed( sc, "rtems_semaphore_flush" );

    sc = rtems_task_wake_after( 2 );
    directive_failed( sc, "rtems_task_wake_after" );
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION 09 ***" );

  puts( "Init - Trying to generate timeout from ISR while blocking" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( sc, "rtems_semaphore_create of SM1" );

  Main_task = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name( 'F', 'L', 'S', 'H' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Flusher_id
  );
  directive_failed( sc, "rtems_task_create" );

  sc = rtems_task_start( Flusher_id, Flusher, 0 );
  directive_failed( sc, "rtems_task_start" );

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  case_hit = false;

  while (!case_hit) {
    interrupt_critical_section_test_support_delay();

    (void) rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
  }

  puts( "Init - Case hit" );

  puts( "*** END OF TEST INTERRUPT CRITICAL SECTION 09 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       2
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  500

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
