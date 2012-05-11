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

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine test_operation_from_isr(rtems_id timer, void *arg);

volatile bool operation_performed_from_tsr;

void *Pointer1;

rtems_timer_service_routine test_operation_from_isr(
  rtems_id  timer,
  void     *arg
)
{
  /* free memory from ISR so it is deferred */
  free( Pointer1 );

  operation_performed_from_tsr = true;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code     status;
  rtems_id              timer;
  void                 *pointer2;

  puts( "\n\n*** TEST MALLOC 02 ***" );

  puts( "malloc memory to free from ISR" );
  Pointer1 = malloc( 20 );

  /*
   *  Timer used in multiple ways
   */
  status = rtems_timer_create( rtems_build_name('T', 'M', 'R', '0'), &timer );
  directive_failed( status, "rtems_timer_create" );

  operation_performed_from_tsr = false;

  /*
   * Test Operation from ISR
   */
  status = rtems_timer_fire_after( timer, 10, test_operation_from_isr, NULL );
  directive_failed( status, "timer_fire_after failed" );

  /* delay to let timer fire */
  status = rtems_task_wake_after( 20 );
  directive_failed( status, "timer_wake_after failed" );

  if ( !operation_performed_from_tsr ) {
    puts( "Operation from ISR did not get processed\n" );
    rtems_test_exit( 0 );
  }

  puts( "Free from ISR successfully processed" );
  puts( "Now malloc'ing more memory to process the free" );
  pointer2 = malloc(20);
  rtems_test_assert( pointer2 );

  puts( "*** END OF TEST MALLOC 02 ***" );
  rtems_test_exit( 0 );
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MALLOC_DIRTY

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_MAXIMUM_TIMERS            1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */

