/*  timer.c
 *
 *  This file manages the interval timer on the PA-RISC.
 *
 *  NOTE: It is important that the timer start/stop overhead be
 *        determined when porting or modifying this code.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

/* For solaris 2.4 */
#define __EXTENSIONS__

#include <bsp.h>
#include <time.h>
#include <sys/time.h>

struct timeval  Timer_start;
struct timeval  Timer_stop;
struct timezone Time_zone;

rtems_boolean   Timer_driver_Find_average_overhead;

void Timer_initialize()
{
   gettimeofday( &Timer_start, &Time_zone );
}

#define AVG_OVERHEAD      0  /* It typically takes xxx microseconds */
                             /* (XX countdowns) to start/stop the timer. */
#define LEAST_VALID       10 /* Don't trust a value lower than this */

int Read_timer()
{
  int total;

  gettimeofday( &Timer_stop, &Time_zone );

  if ( Timer_stop.tv_sec == Timer_start.tv_sec )
    total = Timer_stop.tv_usec - Timer_start.tv_usec;
  else {
    total  = 1000000 - Timer_start.tv_usec;
    total += (Timer_stop.tv_sec - Timer_start.tv_sec - 1) * 1000000;
    total += Timer_stop.tv_usec;
  }

  if ( Timer_driver_Find_average_overhead == 1 )
      return total;          /* in countdown units */
  else {
    if ( total < LEAST_VALID )
        return 0;            /* below timer resolution */
    return total - AVG_OVERHEAD;
  }
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
