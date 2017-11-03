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

#if defined(FIRE_AFTER)
  #define TEST_NAME          "13"
  #define TEST_STRING        "Timer Fire After"
  #define TEST_DIRECTIVE     rtems_timer_fire_after

#elif defined(SERVER_FIRE_AFTER)
  #define TEST_NAME          "14"
  #define TEST_STRING        "Timer Server Fire After"
  #define TEST_DIRECTIVE     rtems_timer_server_fire_after

#else
  #error "Test Mode not defined"
#endif

const char rtems_test_name[] = "SPINTRCRITICAL " TEST_NAME;

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_release_from_isr(rtems_id  timer, void *arg);
rtems_timer_service_routine TimerMethod(rtems_id  timer, void *arg);

rtems_id Timer;

rtems_timer_service_routine TimerMethod(
  rtems_id  timer,
  void     *arg
)
{
}

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  (void) rtems_timer_fire_after( Timer, 10, TimerMethod, NULL );
}

static bool test_body( void *arg )
{
  rtems_status_code sc;

  (void) arg;

  sc = TEST_DIRECTIVE( Timer, 10, TimerMethod, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  return false;
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;

  TEST_BEGIN();

  puts( "Init - Trying to generate timer fire from ISR while firing" );
  puts( "Init - Variation is: " TEST_STRING );

  puts( "Init - There is no way for the test to know if it hits the case" );

  #if defined(SERVER_FIRE_AFTER)
   /* initiate timer server */
    sc = rtems_timer_initiate_server(
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES
    );
    directive_failed( sc, "rtems_timer_initiate_server" );
  #endif

  puts( "Init - rtems_timer_create - OK" );
  sc = rtems_timer_create( rtems_build_name( 'P', 'E', 'R', '1' ), &Timer);
  directive_failed( sc, "rtems_timer_create" );

  interrupt_critical_section_test( test_body, NULL, test_release_from_isr );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#if defined(SERVER_FIRE_AFTER)
  #define CONFIGURE_MAXIMUM_TASKS     3
#else
  #define CONFIGURE_MAXIMUM_TASKS     2
#endif
#define CONFIGURE_MAXIMUM_TIMERS      2
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
