/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/timerdrv.h>

pthread_mutex_t MutexId;

void *Blockers(
  void *argument
)
{
  int status;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

  status = pthread_mutex_lock( &MutexId );
  rtems_test_assert( status == 0 );

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  pthread_t  threadId;
  long       end_time;

  puts( "\n\n*** POSIX TIME TEST MUTEX 05 ***" );

  for ( i=0 ; i < OPERATION_COUNT ; i++ ) {
    status = pthread_create( &threadId, NULL, Blockers, NULL );
    rtems_test_assert( status == 0 );
  }
  
  /*
   * Deliberately create the mutex after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   */
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert( status == 0 );

  /*
   * Ensure the mutex is unavailable so the other threads block.
   */
  status = pthread_mutex_lock( &MutexId );
  rtems_test_assert( status == 0 );

  /*
   * Let the other threads start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other threads run */

  benchmark_timer_initialize();
    status = pthread_mutex_unlock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutex_unlock (no preemption)",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST MUTEX 05 ***" );
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 1
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
