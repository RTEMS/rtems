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
#include <time.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);
rtems_timer_service_routine test_release_from_isr(rtems_id timer, void *arg);

#define TEST_NAME          "01"
#define TEST_STRING        "POSIX Timer"

rtems_id          Main_task;
timer_t           Timer;
struct itimerspec TimerParams;

#define POSIX_TIMER_RELATIVE 0

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  (void) timer_settime(Timer, POSIX_TIMER_RELATIVE, &TimerParams, NULL);
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  int    sc;
  int    resets;

  puts( "\n\n*** TEST POSIX INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );

  puts( "Init - Trying to generate timer fire from ISR while firing" );
  puts( "Init - Variation is: " TEST_STRING );

  puts( "Init - There is no way for the test to know if it hits the case" );

  /* create POSIX Timer */
  sc = timer_create (CLOCK_REALTIME, NULL, &Timer);
  if ( sc == -1 ) {
    perror ("Error in timer creation\n");
    rtems_test_exit(0);
  }

  Main_task = rtems_task_self();

  /* we don't care if it ever fires */
  TimerParams.it_interval.tv_sec  = 10;
  TimerParams.it_interval.tv_nsec = 0;
  TimerParams.it_value.tv_sec     = 10;
  TimerParams.it_value.tv_nsec    = 0;

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  for (resets=0 ; resets<10 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    sc = timer_settime(Timer, POSIX_TIMER_RELATIVE, &TimerParams, NULL);
    if ( sc == -1 ) {
      perror ("Error in timer setting\n");
      rtems_test_exit(0);
    }

  }

  puts( "*** END OF TEST POSIX INTERRUPT CRITICAL SECTION " TEST_NAME " ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_TIMERS         1
#define CONFIGURE_MAXIMUM_POSIX_TIMERS   1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
