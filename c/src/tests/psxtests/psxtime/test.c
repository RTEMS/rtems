/*
 *  This test exercises the time of day services via the Classic
 *  and POSIX APIs to make sure they are consistent.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <rtems.h>
#include <rtems/libio.h>

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
  assert( !status );
 
  do {
    status = rtems_clock_get( RTEMS_CLOCK_GET_TOD, &new_tod );
    assert( !status );
    print_time( "rtems_clock_get          ", &new_tod, "\n" );

    /* now do the posix time gets */
    result = gettimeofday( &tv, 0 );
    assert( result == 0 );
    a_time_t = tv.tv_sec;   /* ctime() takes a time_t */
    printf( "gettimeofday: %s", ctime( &a_time_t) );
  
    a_time_t = time( 0 );
    printf( "time:         %s", ctime( &a_time_t ) );

    a_tm = localtime( &a_time_t );
    printf( "localtime:    %s", asctime( a_tm ) );

    a_tm = gmtime( &a_time_t );
    printf( "gmtime:       %s\n",  asctime( a_tm ) );

    status = rtems_task_wake_after( 5 * TICKS_PER_SECOND );

    i++;

  } while( i < 6 );
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  int i;

  puts( "\n\n*** POSIX TIME OF DAY TEST ***" );

  i = 0;
  while ( i < NUMBER_OF_DATES ) {
    check_a_tod( &Dates[i] );
    i++;
  }

  puts( "\n\n*** END OF TIME OF DAY TEST 01 ***" );
  rtems_test_exit(0);
}

