/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <intrcritical.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_task Secondary_task(rtems_task_argument ignored);

#define TEST_NAME          "15"
#define INIT_PRIORITY      2
#define BLOCKER_PRIORITY   1

rtems_id Main_task;
rtems_id Secondary_task_id;
rtems_id Semaphore;

rtems_task Secondary_task(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  while (1) {
    sc = rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
    fatal_directive_status( sc, RTEMS_TIMEOUT, "rtems_semaphore_obtain" );
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  int                   resets;

  puts(
    "\n\n*** TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***\n"
    "Init - Trying to generate timeout of a thread while another is blocking\n"
    "Init -   on the same thread queue\n"
    "Init - There is no way for the test to know if it hits the case"
  );

  puts( "Init - rtems_semaphore_create - OK" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( sc, "rtems_semaphore_create of SM1" );

  puts( "Init - rtems_task_create - OK" );
  sc = rtems_task_create(
    rtems_build_name( 'B', 'L', 'C', 'K' ),
    BLOCKER_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_NO_PREEMPT,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Secondary_task_id
  );
  directive_failed( sc, "rtems_task_create" );

  sc = rtems_task_start( Secondary_task_id, Secondary_task, 0 );
  directive_failed( sc, "rtems_task_start" );

  Main_task = rtems_task_self();

  interrupt_critical_section_test_support_initialize( NULL );

  for (resets=0 ; resets<10 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    sc = rtems_task_restart( Secondary_task_id, 1 );
    directive_failed( sc, "rtems_task_restart" );

    sc = rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 1 );
    fatal_directive_status( sc, RTEMS_TIMEOUT, "rtems_semaphore_obtain" );
  }

  puts( "*** END OF TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          2
#define CONFIGURE_MAXIMUM_SEMAPHORES     1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INIT_TASK_PRIORITY  INIT_PRIORITY
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
