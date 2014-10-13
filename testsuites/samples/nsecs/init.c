/**
 *  @file
 *
 *  Nanoseconds accuracy timestamp test
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define CONFIGURE_INIT
#include "system.h"

#include <rtems/score/timespec.h> /* _Timespec_Substract */

#include "tmacros.h"
#include "pritime.h"

const char rtems_test_name[] = "NANOSECOND CLOCK";

static char *my_ctime( time_t t )
{
  static char b[32];
  ctime_r(&t, b);
  b[ strlen(b) - 1] = '\0';
  return b;
}

static void subtract_em(
  struct timespec *start,
  struct timespec *stop,
  struct timespec *t
)
{
  t->tv_sec = 0;
  t->tv_nsec = 0;
  _Timespec_Subtract( start, stop, t );
}


rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;
  rtems_time_of_day time;
  int index;

  TEST_BEGIN();

  time.year   = 2007;
  time.month  = 03;
  time.day    = 24;
  time.hour   = 11;
  time.minute = 15;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );
  directive_failed( status, "clock set" ); 

  /*
   *  Iterate 10 times showing difference in TOD
   */
  printf( "10 iterations of getting TOD\n" );
  for (index=0 ; index <10 ; index++ ) {
    struct timespec start, stop;
    struct timespec diff;

    clock_gettime( CLOCK_REALTIME, &start );
    clock_gettime( CLOCK_REALTIME, &stop );

    subtract_em( &start, &stop, &diff );
    printf( "Start: %s:%ld\nStop : %s:%ld",
      my_ctime(start.tv_sec), start.tv_nsec,
      my_ctime(stop.tv_sec), stop.tv_nsec
    );

    printf( " --> %" PRIdtime_t ":%ld\n", diff.tv_sec, diff.tv_nsec );
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
    printf( "%" PRIdtime_t ":%ld %" PRIdtime_t ":%ld --> %" PRIdtime_t ":%ld\n",
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
    long j, max = (index * 10000L);
    rtems_clock_get_uptime( &start );
      for (j=0 ; j<max ; j++ )
        dummy_function_empty_body_to_force_call();
    rtems_clock_get_uptime( &stop );

    subtract_em( &start, &stop, &diff );
    printf( "loop of %ld %" PRIdtime_t
              ":%ld %" PRIdtime_t ":%ld --> %" PRIdtime_t ":%ld\n",
      max,
      start.tv_sec, start.tv_nsec,
      stop.tv_sec, stop.tv_nsec,
      diff.tv_sec, diff.tv_nsec
   );
  }

  sleep(1);

  TEST_END();
  exit(0);
}

