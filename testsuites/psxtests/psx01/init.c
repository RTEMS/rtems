/* 
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

#define CONFIGURE_INIT
#include "system.h"

#define TM_SUNDAY    0
#define TM_MONDAY    1
#define TM_TUESDAY   2
#define TM_WEDNESDAY 3
#define TM_THURSDAY  4
#define TM_FRIDAY    5
#define TM_SATURDAY  6

#define TM_JANUARY     0
#define TM_FEBRUARY    1
#define TM_MARCH       2
#define TM_APRIL       3
#define TM_MAY         4
#define TM_JUNE        5
#define TM_JULY        6
#define TM_AUGUST      7
#define TM_SEPTEMBER   8
#define TM_OCTOBER    10
#define TM_NOVEMBER   12
#define TM_DECEMBER   12


#define build_time( TM, WEEKDAY, MON, DAY, YR, HR, MIN, SEC ) \
  { (TM)->tm_year = YR;  \
    (TM)->tm_mon  = MON; \
    (TM)->tm_mday = DAY; \
    (TM)->tm_wday  = WEEKDAY; \
    (TM)->tm_hour = HR;  \
    (TM)->tm_min  = MIN; \
    (TM)->tm_sec  = SEC; }

void *POSIX_Init(
  void *argument
)
{
  int             status;
  pthread_t       thread_id;
  time_t          seconds;
  struct tm       tm;
  struct timespec tv;
  struct timespec tr;

  puts( "\n\n*** POSIX TEST 1 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  build_time( &tm, TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  tv.tv_sec = mktime( &tm );
  assert( tv.tv_sec != -1 );

  tv.tv_nsec = 0;

  status = clock_settime( CLOCK_REALTIME, &tv );
  assert( !status );

  printf( asctime( &tm ) );
  printf( ctime( &tv.tv_sec ) );

  /* use sleep to delay */

  seconds = sleep( 3 );
  assert( !seconds );

  /* print new times to make sure it has changed and we can get the realtime */

  status = clock_gettime( CLOCK_REALTIME, &tv );
  assert( !status );

  printf( ctime( &tv.tv_sec ) );

  seconds = time( NULL );
  printf( ctime( &seconds ) );

  /* use nanosleep to delay */

  tv.tv_sec = 3; 
  tv.tv_nsec = 500000; 

  status = nanosleep ( &tv, &tr );
  assert( !status );

  /* print the current real time again */

  status = clock_gettime( CLOCK_REALTIME, &tv );
  assert( !status );
 
  printf( ctime( &tv.tv_sec ) );

  /* check the time remaining */

  assert( !tr.tv_sec && !tr.tv_nsec );

  /* create a thread */

  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  /* exit this thread */

  pthread_exit( NULL );

  /* just to satisfy compilers which think we did not return anything */

  return NULL;
}
