/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"

rtems_task Task_1_through_3(
  rtems_task_argument index
)
{
  rtems_time_of_day time;
  rtems_status_code status;
  rtems_interval    ticks;
  rtems_name        name;

  status = rtems_object_get_classic_name( rtems_task_self(), &name );
  directive_failed( status, "rtems_object_get_classic_name" );

  /*
   * Use TOD_MILLISECONDS_TO_TICKS not RTEMS_MILLISECONDS_TO_TICKS to
   * test C implementation in SuperCore -- not macro version used
   * everywhere else.
   */
  ticks = TOD_MILLISECONDS_TO_TICKS( index * 5 * 1000 );

  while( FOREVER ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    if ( time.second >= 35 ) {
      puts( "*** END OF TEST 1 ***" );
      rtems_test_exit( 0 );
    }

    put_name( name, FALSE );
    print_time( " - rtems_clock_get_tod - ", &time, "\n" );

    status = rtems_task_wake_after( ticks );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
