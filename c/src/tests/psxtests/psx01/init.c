/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>
#include <sys/utsname.h>

void *POSIX_Init(
  void *argument
)
{
  struct timespec tv;
  struct timespec tr;
  int             status;
  int             priority;
  pthread_t       thread_id;
  time_t          seconds;
  time_t          seconds1;
  time_t          remaining;
  struct tm       tm;
  struct utsname  uts;

  puts( "\n\n*** POSIX TEST 1 ***" );

  build_time( &tm, TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* print some system information */

  puts( "Init: uname - EFAULT (invalid uts pointer argument)" );
  status = uname( NULL );
  assert( status == -1 );
  assert( errno == EFAULT );
   
  status = uname( &uts );
  assert( !status );
  printf( "Init: uts.sysname: %s\n", uts.sysname );
  printf( "Init: uts.nodename: %s\n", uts.nodename );
  printf( "Init: uts.release: %s\n", uts.release );
  printf( "Init: uts.version: %s\n", uts.version );
  printf( "Init: uts.machine: %s\n", uts.machine );
  puts("");
   
  /* error cases in clock_gettime and clock_settime */

  puts( "Init: clock_gettime - EINVAL (invalid clockid)" );
  status = clock_settime( -1, &tv );
  assert( status == -1 );
  assert( errno == EINVAL );

  puts( "Init: clock_settime - EINVAL (invalid clockid)" );
  status = clock_settime( -1, &tv );
  assert( status == -1 );
  assert( errno == EINVAL );

  /* exercise clock_getres */

  puts( "Init: clock_getres - EINVAL (invalid clockid)" );
  status = clock_getres( -1, &tv );
  assert( status == -1 );
  assert( errno == EINVAL );

  puts( "Init: clock_getres - EINVAL (NULL resolution)" );
  status = clock_getres( CLOCK_REALTIME, NULL );
  assert( status == -1 );
  assert( errno == EINVAL );

  puts( "Init: clock_getres - SUCCESSFUL" );
  status = clock_getres( CLOCK_REALTIME, &tv );
  printf( "Init: resolution = sec (%ld), nsec (%ld)\n", tv.tv_sec, tv.tv_nsec );
  assert( !status );

  /* set the time of day, and print our buffer in multiple ways */

  tv.tv_sec = mktime( &tm );
  assert( tv.tv_sec != -1 );

  tv.tv_nsec = 0;

  /* now set the time of day */

  empty_line();

  printf( asctime( &tm ) );
  puts( "Init: clock_settime - SUCCESSFUL" );
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

  /*  just to have the value copied out through the parameter */
  
  seconds = time( &seconds1 );
  assert( seconds == seconds1 );

  /* check the time remaining */

  printf( "Init: seconds remaining (%d)\n", (int)remaining );
  assert( !remaining );

  /* error cases in nanosleep */

  empty_line();
  puts( "Init: nanosleep - EINVAL (NULL time)" );
  status = nanosleep ( NULL, &tr );
  assert( status == -1 );
  assert( errno == EINVAL );

  tv.tv_sec = 0;
  tv.tv_nsec = TOD_NANOSECONDS_PER_SECOND * 2;
  puts( "Init: nanosleep - EINVAL (too many nanoseconds)" );
  status = nanosleep ( &tv, &tr );
  assert( status == -1 );
  assert( errno == EINVAL );

  /* this is actually a small delay or yield */
  tv.tv_sec = -1;
  tv.tv_nsec = 0;
  puts( "Init: nanosleep - negative seconds small delay " );
  status = nanosleep ( &tv, &tr );
  assert( !status );

  /* use nanosleep to yield */

  tv.tv_sec = 0; 
  tv.tv_nsec = 0; 

  puts( "Init: nanosleep - yield" ); 
  status = nanosleep ( &tv, &tr );
  assert( !status );
  assert( !tr.tv_sec );
  assert( !tr.tv_nsec );

  /* use nanosleep to delay */

  tv.tv_sec = 3; 
  tv.tv_nsec = 500000; 

  puts( "Init: nanosleep - 3.05 seconds" ); 
  status = nanosleep ( &tv, &tr );
  assert( !status );

  /* print the current real time again */

  status = clock_gettime( CLOCK_REALTIME, &tv );
  assert( !status );
 
  printf( ctime( &tv.tv_sec ) );

  /* check the time remaining */

  printf( "Init: sec (%ld), nsec (%ld) remaining\n", tr.tv_sec, tr.tv_nsec );
  assert( !tr.tv_sec && !tr.tv_nsec );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init: ID is 0x%08x\n", Init_id );

  /* exercise get minimum priority */

  priority = sched_get_priority_min( SCHED_FIFO );
  printf( "Init: sched_get_priority_min (SCHED_FIFO) -- %d\n", priority );
  assert( priority != -1 );

  puts( "Init: sched_get_priority_min -- EINVAL (invalid policy)" );
  priority = sched_get_priority_min( -1 );
  assert( priority == -1 );
  assert( errno == EINVAL );

  /* exercise get maximum priority */
 
  priority = sched_get_priority_max( SCHED_FIFO );
  printf( "Init: sched_get_priority_max (SCHED_FIFO) -- %d\n", priority );
  assert( priority != -1 );

  puts( "Init: sched_get_priority_min -- EINVAL (invalid policy)" );
  priority = sched_get_priority_min( -1 );
  assert( priority == -1 );
  assert( errno == EINVAL );

  /* print the round robin time quantum */
 
  status = sched_rr_get_interval( getpid(), &tr );
  printf(
    "Init: Round Robin quantum is %ld seconds, %ld nanoseconds\n",
    tr.tv_sec,
    tr.tv_nsec
  );
  assert( !status );
  
  /* create a thread */

  puts( "Init: pthread_create - SUCCESSFUL" );
  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  /* too may threads error */

  puts( "Init: pthread_create - EAGAIN (too many threads)" );
  status = pthread_create( &thread_id, NULL, Task_1_through_3, NULL );
  assert( status == EAGAIN );

  puts( "Init: sched_yield to Task_1" );
  status = sched_yield();
  assert( !status );

    /* switch to Task_1 */

  /* exit this thread */

  puts( "Init: pthread_exit" );
  pthread_exit( NULL );

    /* switch to Task_1 */

  return NULL; /* just so the compiler thinks we returned something */
}
