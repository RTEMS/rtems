/* 
 *  This file implements a stub benchmark timer that is sufficient to
 *  satisfy linking the RTEMS Benchmarks.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *
 *  $Id$
 */

#include <bsp.h>

rtems_boolean Timer_driver_Find_average_overhead;

void Timer_initialize()
{
}
int Read_timer()
{
  if (Timer_driver_Find_average_overhead)
    return 1;
  return 0;
}

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
