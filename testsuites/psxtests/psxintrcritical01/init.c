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
#include <time.h>

const char rtems_test_name[] = "PSXINTRCRITICAL 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument ignored);

#define TEST_NAME          "01"
#define TEST_STRING        "POSIX Timer"

static timer_t           Timer;
static struct itimerspec TimerParams;

#define POSIX_TIMER_RELATIVE 0

static bool test_body( void *arg )
{
  int rv;

  (void) arg;

  rv = timer_settime(Timer, POSIX_TIMER_RELATIVE, &TimerParams, NULL);
  rtems_test_assert( rv == 0 );

  return false;
}

static rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  test_body( NULL );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  int sc;

  TEST_BEGIN();

  puts( "Init - Trying to generate timer fire from ISR while firing" );
  puts( "Init - Variation is: " TEST_STRING );

  puts( "Init - There is no way for the test to know if it hits the case" );

  /* create POSIX Timer */
  sc = timer_create (CLOCK_REALTIME, NULL, &Timer);
  if ( sc == -1 ) {
    perror ("Error in timer creation\n");
    rtems_test_exit(0);
  }

  /* we don't care if it ever fires */
  TimerParams.it_interval.tv_sec  = 10;
  TimerParams.it_interval.tv_nsec = 0;
  TimerParams.it_value.tv_sec     = 10;
  TimerParams.it_value.tv_nsec    = 0;

  interrupt_critical_section_test( test_body, NULL, test_release_from_isr );

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS          1
#define CONFIGURE_MAXIMUM_TIMERS         1
#define CONFIGURE_MAXIMUM_POSIX_TIMERS   1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
