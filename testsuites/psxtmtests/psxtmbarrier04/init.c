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

const char rtems_test_name[] = "PSXTMBARRIER 04";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

#define N  2
pthread_barrier_t     barrier;

void *Blocker(
  void *argument
)
{
  benchmark_timer_t end_time;

  /* block on barrier wait, switch back to first thread */
  (void) pthread_barrier_wait( &barrier );


  /* preempt first thread and stop time */
  end_time = benchmark_timer_read();
  put_time(
    "pthread_barrier_wait: releasing preempt",
    end_time,
    1,
    0,
    0
  );
  TEST_END();
  rtems_test_exit( 0 );

}

void *POSIX_Init(
  void *argument
)
{
  int        status;
  pthread_t  threadId;
  int policy;
  struct sched_param param;

  TEST_BEGIN();

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  status = pthread_barrier_init( &barrier, NULL, N );
  rtems_test_assert( status == 0 );

  /* yield to second thread so it can start up and block */
  sched_yield();

  pthread_getschedparam(threadId, &policy, &param);
  param.sched_priority = sched_get_priority_max(policy) - 1;
  /* preempt doesn't occur here due to second thread being blocked */
  pthread_setschedparam(threadId, policy, &param);

  /* as soon as this thread waits on barrier, release occurs, 2nd thread
   * preempts this one due to having a higher priority
   */
  benchmark_timer_initialize();
  status = pthread_barrier_wait( &barrier );

  /* avoid warning but should not be executed */
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
