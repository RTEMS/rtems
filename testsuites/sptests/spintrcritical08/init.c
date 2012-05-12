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
rtems_rate_monotonic_period_states getState(void);

rtems_id Main_task;
rtems_id Period;
volatile bool case_hit;

rtems_rate_monotonic_period_states getState(void)
{
  Objects_Locations       location;
  Rate_monotonic_Control *period;

  period = (Rate_monotonic_Control *)_Objects_Get(
    &_Rate_monotonic_Information, Period, &location );
  if ( location != OBJECTS_LOCAL ) {
    puts( "Bad object lookup" );
    rtems_test_exit(0);
  }
  _Thread_Unnest_dispatch();

  return period->state;
}

rtems_timer_service_routine test_release_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  if ( getState() == RATE_MONOTONIC_EXPIRED_WHILE_BLOCKING )
    case_hit = true;
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code     sc;
  int                   resets;

  puts( "\n\n*** TEST INTERRUPT CRITICAL SECTION 08 ***" );

  puts( "Init - Trying to generate period ending while blocking" );

  puts( "Init - rtems_rate_monotonic_create - OK" );
  sc = rtems_rate_monotonic_create(
    rtems_build_name( 'P', 'E', 'R', '1' ),
    &Period
  );
  directive_failed( sc, "rtems_rate_monotonic_create" );

  Main_task = rtems_task_self();

  interrupt_critical_section_test_support_initialize( test_release_from_isr );

  case_hit = false;

  for (resets=0 ; case_hit == false && resets< 2 ;) {
    if ( interrupt_critical_section_test_support_delay() )
      resets++;

    sc = rtems_rate_monotonic_period( Period, 1 );
    if ( sc == RTEMS_TIMEOUT )
      continue;
    directive_failed( sc, "rtems_monotonic_period");
  }

  if ( case_hit ) {
    puts( "Init - It appears the case has been hit" );
    puts( "*** END OF TEST INTERRUPT CRITICAL SECTION 08 ***" );
  } else
    puts( "Init - Case not hit - ran too long" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       2
#define CONFIGURE_MAXIMUM_TIMERS      1
#define CONFIGURE_MAXIMUM_PERIODS     1
#define CONFIGURE_MICROSECONDS_PER_TICK  1000
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
