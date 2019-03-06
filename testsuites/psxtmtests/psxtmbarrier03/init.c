/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "PSXTMBARRIER 03";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

#define N  2
pthread_barrier_t     barrier;

void *Blocker(
  void *argument
)
{
  (void) pthread_barrier_wait( &barrier );
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

  TEST_BEGIN();

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );

  /*
   * Deliberately create the barrier after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   * The barrier will be released on the Nth thread blocking.
   */
  status = pthread_barrier_init( &barrier, NULL, N );
  rtems_test_assert( status == 0 );

  /*
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other thread run */

  /*
   * Because this is the Nth thread at the barrier, this is an
   * unblocking operation.
   */
  benchmark_timer_initialize();
  status = pthread_barrier_wait( &barrier );
  end_time = benchmark_timer_read();
  /*
   * Upon successful completion return value, the status should be
   * PTHREAD_BARRIER_SERIAL_THREAD.
   */
  rtems_test_assert( status == PTHREAD_BARRIER_SERIAL_THREAD );

  put_time(
    "pthread_barrier_wait: releasing no preempt",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
