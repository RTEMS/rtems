/*
 *  COPYRIGHT (c) 1989-2012.
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 9";

static Thread_Control *thread;

static rtems_id Semaphore;

static bool case_hit;

static bool is_interrupt_timeout(void)
{
  Thread_Wait_flags flags = _Thread_Wait_flags_get( thread );

  return flags == ( THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_READY_AGAIN );
}

static rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();
  Watchdog_Header *header = &cpu_self->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  Watchdog_Control *watchdog = (Watchdog_Control *) header->first;

  if (
    watchdog != NULL
      && watchdog->expire == cpu_self->Watchdog.ticks
      && watchdog->routine == _Thread_Timeout
  ) {
    _Watchdog_Per_CPU_remove( watchdog, cpu_self, header );

    (*watchdog->routine)( watchdog );

    if ( is_interrupt_timeout() ) {
      case_hit = true;
    }
  }
}

static bool test_body( void *arg )
{
  (void) arg;

  rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 1 );

  return case_hit;
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  TEST_BEGIN();

  thread = _Thread_Get_executing();

  puts( "Init - Test may not be able to detect case is hit reliably" );
  puts( "Init - Trying to generate timeout from ISR while blocking" );
  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'M', '1', ' ' ),
    0,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore
  );
  directive_failed( sc, "rtems_semaphore_create of SM1" );

  interrupt_critical_section_test( test_body, NULL, test_release_from_isr );

  if ( case_hit ) {
    puts( "Init - It appears the case has been hit" );
    TEST_END();
  } else
    puts( "Init - Case not hit - ran too long" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_SEMAPHORES  1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
