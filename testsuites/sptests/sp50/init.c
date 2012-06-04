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
#include "pritime.h"

/* forward declarations to avoid warnings */
rtems_timer_service_routine Timer_Routine(rtems_id id, void *ignored);
rtems_task Init(rtems_task_argument argument);

volatile int   Fired;
volatile bool  timerRan;

rtems_timer_service_routine Timer_Routine(rtems_id id, void *ignored)
{
  rtems_status_code status;

  Fired++;
  timerRan = true;

  status = rtems_timer_server_fire_after(
    id,
    rtems_clock_get_ticks_per_second(),
    Timer_Routine,
    NULL
  );
  directive_failed( status, "fire after" );

}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code sc;
  rtems_id          timer1;
  struct timespec   uptime;

  puts( "\n\n*** TEST 50 ***" );

  sc = rtems_timer_initiate_server(
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( sc, "rtems_timer_initiate_server" );

  sc = rtems_timer_create(rtems_build_name( 'T', 'M', 'R', '1' ), &timer1);
  directive_failed( sc, "rtems_timer_create" );

  Fired = 0;
  timerRan = false;

  Timer_Routine(timer1, NULL);

  while (1) {
    sc = rtems_task_wake_after( 10 );
    directive_failed( sc, "rtems_task_wake_after" );

    if ( timerRan == true ) {
      timerRan = false;

      sc = rtems_clock_get_uptime( &uptime );
      directive_failed( sc, "rtems_clock_get_uptime" );

      printf( "Timer fired at %" PRIdtime_t "\n", uptime.tv_sec );
    }

    if ( Fired >= 10 ) {
      puts( "*** END OF TEST 50 ***" );
      rtems_test_exit( 0 );
    }
    /* technically the following is a critical section */
  }
}


/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_INIT
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS         2
#define CONFIGURE_MAXIMUM_TIMERS        1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
