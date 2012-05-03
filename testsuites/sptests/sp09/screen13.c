/*  Screen13
 *
 *  This routine generates error screen 13 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
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

void Screen13()
{
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_io_close( 0xffff, 0x0000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_close with bad major number"
  );
  puts( "TA1 - rtems_io_close - RTEMS_INVALID_NUMBER" );
  status = rtems_io_control( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_close with bad major number"
  );
  puts( "TA1 - rtems_io_control - RTEMS_INVALID_NUMBER" );
  status = rtems_io_initialize( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_initialize with bad major number"
  );
  puts( "TA1 - rtems_io_initialize - RTEMS_INVALID_NUMBER" );
  status = rtems_io_open( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_open with bad major number"
  );
  puts( "TA1 - rtems_io_open - RTEMS_INVALID_NUMBER" );
  status = rtems_io_read( 0xffff, 0x00000, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_read with bad major number"
  );
  puts( "TA1 - rtems_io_read - RTEMS_INVALID_NUMBER" );
  status = rtems_io_write( 0xffff, 0x0ffff, NULL);
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_io_write with bad major number"
  );
  puts( "TA1 - rtems_io_write - RTEMS_INVALID_NUMBER" );

  build_time( &time, 12, 31, 2000, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 1999, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_get_tod" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 2100, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 2099, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );

  build_time( &time, 12, 31, 1991, 23, 59, 59, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_set - ", &time, " - RTEMS_SUCCESSFUL\n" );
  status = rtems_task_wake_after( rtems_clock_get_ticks_per_second() );
  status = rtems_clock_get_tod( &time );
  directive_failed( status, "rtems_clock_set" );
  print_time( "TA1 - rtems_clock_get_tod - ", &time, " - RTEMS_SUCCESSFUL\n" );
}
