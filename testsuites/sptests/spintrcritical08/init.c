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
#include <rtems/score/watchdogimpl.h>
#include <rtems/rtems/ratemonimpl.h>

const char rtems_test_name[] = "SPINTRCRITICAL 8";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static rtems_id Period;

static volatile bool case_hit = false;

static Thread_Control *thread;

static rtems_rate_monotonic_period_states getState(void)
{
  Rate_monotonic_Control *the_period;
  ISR_lock_Context        lock_context;

  the_period = _Rate_monotonic_Get( Period, &lock_context );
  rtems_test_assert( the_period != NULL );
  _ISR_lock_ISR_enable( &lock_context );

  return the_period->state;
}

static rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  Per_CPU_Control *cpu = _Per_CPU_Get();
  Watchdog_Header *header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  Watchdog_Control *watchdog = (Watchdog_Control *) header->first;

  if (
    watchdog != NULL
      && watchdog->expire == cpu->Watchdog.ticks
      && watchdog->routine == _Rate_monotonic_Timeout
  ) {
    Thread_Wait_flags flags = _Thread_Wait_flags_get( thread );

    _Watchdog_Per_CPU_remove_ticks( watchdog );

    rtems_test_assert( getState() == RATE_MONOTONIC_ACTIVE );

    (*watchdog->routine)( watchdog );

    if ( flags == RATE_MONOTONIC_INTEND_TO_BLOCK ) {
      rtems_test_assert(
        _Thread_Wait_flags_get( thread ) == RATE_MONOTONIC_READY_AGAIN
      );
      rtems_test_assert( getState() == RATE_MONOTONIC_ACTIVE );
      case_hit = true;
    }
  }
}

static bool test_body( void *arg )
{
  rtems_status_code sc;

  (void) arg;

  sc = rtems_rate_monotonic_cancel( Period );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_rate_monotonic_period( Period, 1 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_rate_monotonic_period( Period, 1 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL || sc == RTEMS_TIMEOUT );

  return case_hit;
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  TEST_BEGIN();

  puts( "Init - Trying to generate period ending while blocking" );

  thread = _Thread_Get_executing();

  puts( "Init - rtems_rate_monotonic_create - OK" );
  sc = rtems_rate_monotonic_create(
    rtems_build_name( 'P', 'E', 'R', '1' ),
    &Period
  );
  directive_failed( sc, "rtems_rate_monotonic_create" );

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

#define CONFIGURE_MAXIMUM_TASKS       2
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_PERIODS     1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
