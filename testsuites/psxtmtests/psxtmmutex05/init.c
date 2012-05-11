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

#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/timerdrv.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

pthread_mutex_t MutexId;

void *Blocker(
  void *argument
)
{
  (void) pthread_mutex_lock( &MutexId );
  /* should never return */
  rtems_test_assert( FALSE );

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        status;
  pthread_t  threadId;
  benchmark_timer_t end_time;

  puts( "\n\n*** POSIX TIME TEST PSXTMMUTEX05 ***" );

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  
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
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other thread run */

  benchmark_timer_initialize();
    status = pthread_mutex_unlock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutex_unlock - unblocking, no preemption",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST PSXTMMUTEX05 ***" );
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
