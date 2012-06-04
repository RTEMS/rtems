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

#include <tmacros.h>
#include <intrcritical.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_release_from_isr(rtems_id  timer, void *arg);

rtems_id Main_task;

#if defined(EVENT_ANY)
  #define TEST_NAME          "11"
  #define TEST_STRING        "Event Any condition"
  #define EVENTS_TO_SEND     0x1
  #define EVENTS_TO_RECEIVE  0x3

#elif defined(EVENT_ALL)
  #define TEST_NAME          "12"
  #define TEST_STRING        "Event All condition"
  #define EVENTS_TO_SEND     0x3
  #define EVENTS_TO_RECEIVE  0x3

#else
  #error "Test Mode not defined"

#endif

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  (void) rtems_event_send( Main_task, EVENTS_TO_SEND );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_event_set       out;
  int                   resets;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );

  puts( "Init - Test may not be able to detect case is hit reliably" );
  puts( "Init - Trying to generate event send from ISR while blocking" );
  puts( "Init - Variation is: " TEST_STRING );

  Main_task = rtems_task_self();

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  for (resets=0 ; resets< 2 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    (void) rtems_event_receive( EVENTS_TO_RECEIVE, RTEMS_EVENT_ANY, 1, &out );
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
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MICROSECONDS_PER_TICK  1000

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
