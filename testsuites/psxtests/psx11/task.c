/**
 *  @file
 *
 *  Task_1
 *
 *  This routine serves as a test task.  It verifies the basic task
 *  switching capabilities of the executive.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <time.h>
#include <sched.h>

/* forward declarations to avoid warnings */
void diff_timespec(
  struct timespec *start,
  struct timespec *stop,
  struct timespec *result
);

void diff_timespec(
  struct timespec *start,
  struct timespec *stop,
  struct timespec *result
)
{
   const long nsecs_per_sec = 1000000000;

   result->tv_sec = stop->tv_sec - start->tv_sec;
   if ( stop->tv_nsec < start->tv_nsec ) {
      result->tv_nsec = nsecs_per_sec - start->tv_nsec + stop->tv_nsec;
      result->tv_sec--;
   } else
      result->tv_nsec = stop->tv_nsec - start->tv_nsec;

}

void *Task_1(
  void *argument
)
{
  int status;
  struct timespec start;
  struct timespec current;
  struct timespec difference;
  struct timespec delay;

  status = clock_gettime( CLOCK_REALTIME, &start );
  rtems_test_assert( !status );

  status = sched_rr_get_interval( getpid(), &delay );
  rtems_test_assert( !status );

  /* double the rr interval for confidence */

  delay.tv_sec *= 2;
  delay.tv_nsec *= 2;
  if ( delay.tv_nsec >= 1000000000 ) {   /* handle overflow/carry */
    delay.tv_nsec -= 1000000000;
    delay.tv_sec++;
  }


  puts( "Task_1: killing time" );
  for ( ; ; ) {

    status = clock_gettime( CLOCK_REALTIME, &current );
    rtems_test_assert( !status );

    diff_timespec( &start, &current, &difference );

    if ( difference.tv_sec < delay.tv_sec )
      continue;

    if ( difference.tv_sec > delay.tv_sec )
      break;

    if ( difference.tv_nsec > delay.tv_nsec )
      break;

  }

  puts( "Task_1: exitting" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
