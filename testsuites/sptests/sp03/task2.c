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

rtems_task Task_2(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  rtems_name        name;

  status = rtems_object_get_classic_name( rtems_task_self(), &name );
  directive_failed( status, "rtems_object_get_classic_name" );

  while( FOREVER ) {
    status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
    directive_failed( status, "rtems_task_wake_after" );

    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    if ( time.second >= 17 ) {
      puts( "*** END OF TEST 3 *** " );
      rtems_test_exit( 0 );
    }

    put_name( name, FALSE );
    print_time( "  ", &time, "\n" );
  }
}
