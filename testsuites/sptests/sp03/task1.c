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

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  rtems_name        name;

  status = rtems_object_get_classic_name( rtems_task_self(), &name );
  directive_failed( status, "rtems_object_get_classic_name" );

  build_time( &time, 12, 31, 1988, 9, 15, 0, 0 );

  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  put_name( name, FALSE );
  print_time( " sets clock: ", &time, "\n" );

  while( FOREVER ) {
    status = rtems_clock_get_tod( &time );
    directive_failed( status, "rtems_clock_get_tod" );

    put_name( name, FALSE );
    print_time( " going to sleep:  ", &time, "\n" );

    time.second += 5;
    time.minute += ( time.second / 60 );
    time.second %= 60;
    time.hour += ( time.minute / 60 );
    time.minute %= 60;
    time.hour %= 24;

    status = rtems_task_wake_when( &time );
    directive_failed( status, "rtems_task_wake_when" );

    put_name( name, FALSE );
    print_time( " awakened:  ", &time, "\n" );
  }
}
