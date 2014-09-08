/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <intrcritical.h>

#include <rtems/rtems/semimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 16";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_release_from_isr(rtems_id  timer, void *arg);
Thread_blocking_operation_States getState(void);

Thread_Control *Main_TCB;
rtems_id        Semaphore;
volatile bool   case_hit = false;

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
  if ( getState() == THREAD_BLOCKING_OPERATION_NOTHING_HAPPENED ) {
    case_hit = true;
    (void) rtems_semaphore_release( Semaphore );
  }

  if ( Main_TCB->Wait.queue != NULL ) {
    _Thread_queue_Process_timeout( Main_TCB );
  }
}

static bool test_body( void *arg )
{
  rtems_status_code sc;

  (void) arg;

  sc = rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT );

  return case_hit;
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  TEST_BEGIN();
  puts(
    "Init - Trying to generate timeout of a thread that had its blocking\n"
    "Init -   request satisfied while blocking but before time timeout"
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

  Main_TCB  = _Thread_Get_executing();

  interrupt_critical_section_test( test_body, NULL, test_release_from_isr );

  if ( case_hit ) {
    puts( "Init - Case hit" );
    TEST_END();
  } else
    puts( "Init - Case not hit - ran too long" );


  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_TIMERS         1
#define CONFIGURE_MAXIMUM_SEMAPHORES     1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
