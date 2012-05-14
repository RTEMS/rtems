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

#include <timesys.h>
#include <tmacros.h>
#include <rtems/timerdrv.h>
#include "test_support.h"

#include <pthread.h>
#include <sched.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_mutex_t MutexId;

void *Low(
  void *argument
)
{  
  benchmark_timer_t end_time;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

  end_time = benchmark_timer_read();

  put_time(
    "pthread_mutex_timedlock - not available, block",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST PSXTMMUTEX04 ***" );

  rtems_test_exit( 0 );
  return NULL;
}

void *Middle(
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

  status = pthread_mutex_lock( &MutexId);
  rtems_test_assert( !status );  /*this is important*/

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  pthread_t  threadId;

  puts( "\n\n*** POSIX TIME TEST PSXTMMUTEX04 ***" );

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }
  
  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /*
   * Deliberately create the mutex after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   */
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert( !status );

  /*
   * Let the other threads start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other threads run */

  /* start the timer and switch through all the other tasks */
  benchmark_timer_initialize();
  status = pthread_mutex_timedlock( &MutexId, 0 );
  rtems_test_assert( !status );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
