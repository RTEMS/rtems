/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__ 1
#include <tmacros.h>
#include <intrcritical.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_release_from_isr(rtems_id  timer, void *arg);
Thread_blocking_operation_States getState(void);

#if defined(FIFO_NO_TIMEOUT)
  #define TEST_NAME                "01"
  #define TEST_STRING              "FIFO/Without Timeout"
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_DEFAULT_ATTRIBUTES

#elif defined(FIFO_WITH_TIMEOUT)
  #define TEST_NAME                "02"
  #define TEST_STRING              "FIFO/With Timeout"
  #define SEMAPHORE_OBTAIN_TIMEOUT 10
  #define SEMAPHORE_ATTRIBUTES     RTEMS_DEFAULT_ATTRIBUTES

#elif defined(PRIORITY_NO_TIMEOUT)
  #define TEST_NAME                "03"
  #define TEST_STRING              "Priority/Without Timeout"
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#elif defined(PRIORITY_WITH_TIMEOUT)
  #define TEST_NAME                "04"
  #define TEST_STRING              "Priority/With Timeout"
  #define SEMAPHORE_OBTAIN_TIMEOUT 10
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#elif defined(PRIORITY_NO_TIMEOUT_REVERSE)
  #define TEST_NAME                "05"
  #define TEST_STRING              "Priority/Without Timeout (Reverse)"
  #define SEMAPHORE_OBTAIN_TIMEOUT 0
  #define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#else

  #error "Test Mode not defined"
#endif

rtems_id Main_task;
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
  rtems_status_code     status;

  if ( getState() == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) {
    case_hit = true;
  }

  status = rtems_semaphore_release( Semaphore );
  directive_failed_with_level( status, "release", -1 );
}



rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     status;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );

  puts( "Init - Trying to generate semaphore release from ISR while blocking" );
  puts( "Init - Variation is: " TEST_STRING );
  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    1,
    SEMAPHORE_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  Main_task = rtems_task_self();

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  case_hit = false;

  while (!case_hit) {
    interrupt_critical_section_test_support_delay();

    status = rtems_semaphore_obtain(
      Semaphore,
      RTEMS_DEFAULT_OPTIONS,
      SEMAPHORE_OBTAIN_TIMEOUT
    );
    directive_failed( status, "rtems_semaphore_obtain" );
  }

  if ( case_hit ) {
    puts( "Init - Case hit" );
    puts( "*** END OF TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );
  } else
    puts( "Init - Case not hit - ran too long" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#if defined(PRIORITY_NO_TIMEOUT_REVERSE)
  #define CONFIGURE_INIT_TASK_PRIORITY   250
#endif
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
