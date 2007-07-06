/* 
 *  Nanoseconds accuracy timestamp test
 */

/*  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT

#include <rtems.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <rtems/score/timespec.h> /* _Timespec_Substract */

char *my_ctime( time_t t )
{
  static char b[32];
  ctime_r(&t, b);
  b[ strlen(b) - 1] = '\0';
  return b; 
}

void subtract_em(
  struct timespec *start,
  struct timespec *stop,
  struct timespec *t
)
{
  t->tv_sec = 0;
  t->tv_nsec = 0;
  _Timespec_Subtract( start, stop, t );
}

/* body below .. hoping it isn't inlined */
void dummy_function_empty_body_to_force_call();

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  int index;

  puts( "\n\n*** NANOSECOND CLOCK TEST ***" );

  time.year   = 2007;
  time.month  = 03;
  time.day    = 24;
  time.hour   = 11;
  time.minute = 15;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );

  /*
   *  Iterate 10 times showing difference in TOD
   */
  printf( "10 iterations of getting TOD\n" );
  for (index=0 ; index <10 ; index++ ) {
    struct timespec start, stop;
    struct timespec diff;
#if 0
    clock_gettime( CLOCK_REALTIME, &start );
    clock_gettime( CLOCK_REALTIME, &stop );
#else
    _TOD_Get( &start );
    _TOD_Get( &stop );
#endif

    subtract_em( &start, &stop, &diff );
    printf( "Start: %s:%d\nStop : %s:%d",
      my_ctime(start.tv_sec), start.tv_nsec,
      my_ctime(stop.tv_sec), stop.tv_nsec
    );

   printf( " --> %d:%d\n", diff.tv_sec, diff.tv_nsec );
  }

  /*
   *  Iterate 10 times showing difference in Uptime
   */
  printf( "\n10 iterations of getting Uptime\n" );
  for (index=0 ; index <10 ; index++ ) {
    struct timespec start, stop;
    struct timespec diff;
    rtems_clock_get_uptime( &start );
    rtems_clock_get_uptime( &stop );

    subtract_em( &start, &stop, &diff );
    printf( "%d:%d %d:%d --> %d:%d\n",
      start.tv_sec, start.tv_nsec,
      stop.tv_sec, stop.tv_nsec,
      diff.tv_sec, diff.tv_nsec
   );
  }

  /*
   *  Iterate 10 times showing difference in Uptime with different counts
   */
  printf( "\n10 iterations of getting Uptime with different loop values\n" );
  for (index=1 ; index <=10 ; index++ ) {
    struct timespec start, stop;
    struct timespec diff;
    int j, max = (index * 10000);
    rtems_clock_get_uptime( &start );
      for (j=0 ; j<max ; j++ ) 
        dummy_function_empty_body_to_force_call();
    rtems_clock_get_uptime( &stop );

    subtract_em( &start, &stop, &diff );
    printf( "loop of %d %d:%d %d:%d --> %d:%d\n",
      max,
      start.tv_sec, start.tv_nsec,
      stop.tv_sec, stop.tv_nsec,
      diff.tv_sec, diff.tv_nsec
   );
  }

  sleep(1);

  puts( "*** END OF NANOSECOND CLOCK TEST ***" );
  exit(0);
}


#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000
#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/* put here hoping it won't get inlined */
void dummy_function_empty_body_to_force_call() {}
