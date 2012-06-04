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
rtems_task Secondary_task(rtems_task_argument arg);

/* common parameters */
#define SEMAPHORE_ATTRIBUTES     RTEMS_PRIORITY

#if defined(PRIORITY_NO_TIMEOUT_FORWARD)
  #define TEST_NAME          "06"
  #define TEST_STRING        "Priority/Restart Search Task (Forward)"

  #define INIT_PRIORITY      2
  #define BLOCKER_PRIORITY   1
  #define SEMAPHORE_OBTAIN_TIMEOUT 2

#elif defined(PRIORITY_NO_TIMEOUT_REVERSE)
  #define TEST_NAME          "07"
  #define TEST_STRING        "Priority/Restart Search Task (Backward)"
  #define INIT_PRIORITY      126
  #define BLOCKER_PRIORITY   127
  #define SEMAPHORE_OBTAIN_TIMEOUT 0

#else

  #error "Test Mode not defined"
#endif

rtems_id Secondary_task_id;
rtems_id Semaphore;

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  (void) rtems_task_restart( Secondary_task_id, 1 );
}

rtems_task Secondary_task(
  rtems_task_argument arg
)
{
  if ( arg )
    (void) rtems_semaphore_flush( Semaphore );

  (void) rtems_semaphore_obtain(
    Semaphore,
    RTEMS_DEFAULT_OPTIONS,
    RTEMS_NO_TIMEOUT
  );

  rtems_test_assert(0);
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     status;
  int                   resets;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );

  puts( "Init - Trying to generate semaphore release from ISR while blocking" );
  puts( "Init - There is no way for the test to know if it hits the case" );
  puts( "Init - Variation is: " TEST_STRING );
  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    SEMAPHORE_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create of SM1" );

  status = rtems_task_create(
    rtems_build_name( 'B', 'L', 'C', 'K' ),
    BLOCKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Secondary_task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( Secondary_task_id, Secondary_task, 0 );
  directive_failed( status, "rtems_task_start" );

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  for (resets=0 ; resets< 2 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    status = rtems_semaphore_obtain(
      Semaphore,
      RTEMS_DEFAULT_OPTIONS,
      SEMAPHORE_OBTAIN_TIMEOUT
    );
  }

  puts( "*** END OF TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       2
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_INIT_TASK_PRIORITY  INIT_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_MICROSECONDS_PER_TICK  2000
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
