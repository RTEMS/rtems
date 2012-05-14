/*
 *  This test exercises the time of day services via the Classic
 *  and POSIX APIs to make sure they are consistent.
 *
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
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <sys/time.h>

#if !HAVE_DECL_ADJTIME
extern int adjtime(const struct timeval *delta, struct timeval *olddelta);
#endif

extern int _gettimeofday(struct timeval *__p, void *__tz);

void test_adjtime(void);
void check_a_tod(
  rtems_time_of_day *the_tod
);

/*
 *  List of dates and times to test.
 */
#define NUMBER_OF_DATES   8
rtems_time_of_day Dates[ NUMBER_OF_DATES ] = {
  /* YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, TICKS */
  {  1988,   1,    1,   12,    45,     00,     0 },
  {  1988,  12,   31,    9,    00,     00,     0 },
  {  1999,  12,   31,   23,    55,     59,     0 },
  {  1999,  06,   30,   00,    01,     30,     0 },
  {  2000,   1,    1,    0,    15,     59,     0 },
  {  2005,   2,    2,    5,    10,     59,     0 },
  {  2010,   3,    3,   10,     5,     59,     0 },
  {  2020,   4,    4,   15,     0,     59,     0 },
};

/*
 *  Check out a single date and time
 */

void check_a_tod(
  rtems_time_of_day *the_tod
)
{
  rtems_status_code status;
  rtems_time_of_day new_tod;
  time_t            a_time_t;
  struct timeval    tv;
  struct tm        *a_tm;
  int               result;
  int               i = 0;

  print_time( "rtems_clock_set          ", the_tod, "\n" );
  status = rtems_clock_set( the_tod );
  rtems_test_assert( !status );

  do {
    status = rtems_clock_get_tod( &new_tod );
    rtems_test_assert( !status );
    print_time( "rtems_clock_get_tod          ", &new_tod, "\n" );

    /* now do the posix time gets */
    result = gettimeofday( &tv, 0 );
    rtems_test_assert( result == 0 );
    a_time_t = tv.tv_sec;   /* ctime() takes a time_t */
    printf( "gettimeofday: %s", ctime( &a_time_t) );

    a_time_t = time( 0 );
    printf( "time:         %s", ctime( &a_time_t ) );

    a_tm = localtime( &a_time_t );
    printf( "localtime:    %s", asctime( a_tm ) );

    a_tm = gmtime( &a_time_t );
    printf( "gmtime:       %s\n",  asctime( a_tm ) );

    status = rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );

    i++;

  } while( i < 6 );
}

void test_adjtime(void)
{
  int                sc;
  rtems_status_code  status;
  struct timeval     delta;
  struct timeval     olddelta;
  rtems_time_of_day *the_tod;
  rtems_time_of_day  tod;
  rtems_interval     ticks;

  the_tod = &Dates[0];

  print_time( "rtems_clock_set          ", the_tod, "\n" );
  status = rtems_clock_set( the_tod );
  rtems_test_assert( !status );

  delta.tv_sec = 0;
  delta.tv_usec = 0;
  olddelta.tv_sec = 0;
  olddelta.tv_usec = 0;

  puts( "adjtime - NULL delta - EINVAL" );
  sc = adjtime( NULL, &olddelta );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "adjtime - delta out of range - EINVAL" );
  delta.tv_usec = 1000000000; /* 100 seconds worth */
  sc = adjtime( &delta, &olddelta );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "adjtime - delta too small - do nothing" );
  delta.tv_sec = 0;
  delta.tv_usec = 1;
  sc = adjtime( &delta, &olddelta );
  rtems_test_assert( sc == 0 );

  puts( "adjtime - delta too small - do nothing, olddelta=NULL" );
  sc = adjtime( &delta, NULL );
  rtems_test_assert( sc == 0 );

  puts( "adjtime - delta of one second forward, olddelta=NULL" );
  delta.tv_sec = 1;
  delta.tv_usec = 0;
  sc = adjtime( &delta, NULL );
  rtems_test_assert( sc == 0 );

  puts( "adjtime - delta of one second forward" );
  delta.tv_sec = 1;
  delta.tv_usec = 0;
  sc = adjtime( &delta, &olddelta );
  rtems_test_assert( sc == 0 );

  puts( "adjtime - delta of almost two seconds forward" );
  delta.tv_sec = 1;
  delta.tv_usec = 1000000 - 1;
  sc = adjtime( &delta, &olddelta );
  rtems_test_assert( sc == 0 );

  /*
   * spin until over 1/2 of the way to the
   */
  ticks = rtems_clock_get_ticks_per_second();
  rtems_test_assert( ticks );
  ticks /= 2;
  do {
    status = rtems_clock_get_tod( &tod );
    rtems_test_assert( !status );
  } while ( tod.ticks <= ticks );

  puts( "adjtime - delta of almost one second forward which bumps second" );
  delta.tv_sec = 0;
  delta.tv_usec = 1000000 - 1;
  sc = adjtime( &delta, &olddelta );
  rtems_test_assert( sc == 0 );

  status = rtems_clock_get_tod( &tod );
  rtems_test_assert( !status );
  print_time( "rtems_clock_get_tod          ", &tod, "\n" );
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void);

int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  int i;
  int sc;

  puts( "\n\n*** POSIX TIME OF DAY TEST ***" );

  puts( "gettimeofday( NULL, NULL ) - EFAULT" );
  sc = gettimeofday( NULL, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  puts( "_gettimeofday( NULL, NULL ) - EFAULT" );
  sc = _gettimeofday( NULL, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EFAULT );

  test_adjtime();

  /*
   * Now test a number of dates
   */

  i = 0;
  while ( i < NUMBER_OF_DATES ) {
    check_a_tod( &Dates[i] );
    i++;
  }

  puts( "\n\n*** END OF TIME OF DAY TEST 01 ***" );
  rtems_test_exit(0);
}
