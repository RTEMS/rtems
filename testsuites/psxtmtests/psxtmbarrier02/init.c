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

pthread_barrier_t     barrier;

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
    "pthread_barrier_wait – blocking",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF POSIX TIME PSXTMBARRIER 02 ***" );

  rtems_test_exit( 0 );
  return NULL;
}

void *Middle(
  void *argument
)
{
  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

    (void) pthread_barrier_wait( &barrier );
  
  /* should never return */
  rtems_test_assert( FALSE );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  pthread_t  threadId;
  pthread_barrierattr_t attr;

  puts( "\n\n*** POSIX TIME PSXTMBARRIER 02 ***" );

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }
  
  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /*
   * Create the barrier with default attributes and deliberately
   * create it after the Threads so if they run too early, they
   * wull fail with an error.
   */
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  /*
   * Set threshold on count higher than number of threads so all will
   * block.
   */
  status = pthread_barrier_init( &barrier,&attr, OPERATION_COUNT + 2 );
  rtems_test_assert( status == 0 );

  /*
   * Let other threads run through their initialization
   */
  sched_yield();

  /*
   * Start the timer and start the blocking barrier wait chain through
   * all the other tasks.
   */
  benchmark_timer_initialize();
    /* blocking barrier call */
    status = pthread_barrier_wait( &barrier );
  rtems_test_assert( status == 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_MAXIMUM_POSIX_BARRIERS    1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
