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
#include <sched.h>


void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             priority;
  pthread_t       thread_id;
  time_t          seconds;
  time_t          remaining;
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

  remaining = sleep( 3 );
  assert( !remaining );

  /* print new times to make sure it has changed and we can get the realtime */

  status = clock_gettime( CLOCK_REALTIME, &tv );
  assert( !status );

  printf( ctime( &tv.tv_sec ) );

  seconds = time( NULL );
  printf( ctime( &seconds ) );

  /* check the time remaining */

  printf( "seconds remaining (%d)\n", (int)remaining );
  assert( !remaining );

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

  printf( "sec (%d), nsec (%d) remaining\n", (int)tr.tv_sec, (int)tr.tv_nsec );
  assert( !tr.tv_sec && !tr.tv_nsec );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  /* print the minimum priority */

  priority = sched_get_priority_min( SCHED_FIFO );
  printf( "Minimum priority for FIFO is %d\n", priority );
  assert( priority != -1 );

  /* print the maximum priority */
 
  priority = sched_get_priority_max( SCHED_FIFO );
  printf( "Maximum priority for FIFO is %d\n", priority );
  assert( priority != -1 );

  /* print the round robin time quantum */
 
  status = sched_rr_get_interval( getpid(), &tr );
  printf(
    "Round Robin quantum is %d seconds, %d nanoseconds\n",
    (int) tr.tv_sec,
    (int) tr.tv_nsec
  );
  assert( !status );
  
  /* create a thread */

  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  /* exit this thread */

  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
