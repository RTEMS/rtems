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

rtems_task Init(
  rtems_task_argument argument
);
rtems_timer_service_routine Malloc_From_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);

rtems_id   Timer_id[ 3 ];    /* array of timer ids */
rtems_name Timer_name[ 3 ];  /* array of timer names */

volatile int TSR_fired;
volatile void *TSR_malloc_ptr;

rtems_timer_service_routine Malloc_From_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  rtems_status_code  status;

  TSR_fired = 2;
  puts( "TSR: calling malloc" );
  TSR_malloc_ptr = malloc( 64 );

  puts( "TSR: calling free" );
  free( (void *) TSR_malloc_ptr );

  puts( "TSR: delaying with rtems_task_wake_after" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 2 );
  directive_failed( status, "rtems_task_wake_after" );
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code  status;

  puts( "\n\n*** TEST 45 ***" );

  status = rtems_timer_initiate_server(
    RTEMS_TIMER_SERVER_DEFAULT_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( status, "rtems_timer_initiate_server" );

  /*
   * Initialize Timers
   */

  Timer_name[ 1 ] = rtems_build_name( 'T', 'M', '1', ' ' );
  Timer_name[ 2 ] = rtems_build_name( 'T', 'M', '2', ' ' );

  puts( "INIT - rtems_timer_create - creating timer 1" );
  status = rtems_timer_create( Timer_name[ 1 ], &Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_create" );
  printf( "INIT - timer 1 has id (0x%" PRIxrtems_id ")\n", Timer_id[ 1 ] );

  puts( "INIT - rtems_timer_create - creating timer 2" );
  status = rtems_timer_create( Timer_name[ 2 ], &Timer_id[ 2 ] );
  directive_failed( status, "rtems_timer_create" );
  printf( "INIT - timer 2 has id (0x%" PRIxrtems_id ")\n", Timer_id[ 2 ] );

  /*
   *  Schedule malloc TSR for 1 second from now
   */

  TSR_fired = 0;
  TSR_malloc_ptr = (void *) 0xa5a5a5;
  puts( "TA1 - rtems_timer_server_fire_after - timer 1 in 1 seconds" );
  status = rtems_timer_server_fire_after(
    Timer_id[ 1 ],
    1 * rtems_clock_get_ticks_per_second(),
    Malloc_From_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  puts( "TA1 - rtems_task_wake_after - 2 second" );
  status = rtems_task_wake_after( 2 * rtems_clock_get_ticks_per_second() );
  directive_failed( status, "rtems_task_wake_after" );

  if ( TSR_fired == 2 &&
       (TSR_malloc_ptr && TSR_malloc_ptr != (void *)0xa5a5a5) )
    puts( "TSR appears to have executed OK" );
  else {
    printf( "FAILURE ptr=%p TSR_fired=%d\n", TSR_malloc_ptr, TSR_fired );
    rtems_test_exit( 0 );
  }

  /*
   *  Delete timer and exit test
   */
  puts( "TA1 - timer_deleting - timer 1" );
  status = rtems_timer_delete( Timer_id[ 1 ] );
  directive_failed( status, "rtems_timer_delete" );


  puts( "*** END OF TEST 45 *** " );
  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT
/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Two Tasks: Init and Timer Server */
#define CONFIGURE_MAXIMUM_TASKS           2
#define CONFIGURE_MAXIMUM_TIMERS          2
#define CONFIGURE_INIT_TASK_STACK_SIZE    (RTEMS_MINIMUM_STACK_SIZE * 2)

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS       (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <rtems/confdefs.h>

