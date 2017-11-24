/*
 *  COPYRIGHT (c) 2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "PSXTMCOND 04";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

pthread_mutex_t MutexID;
pthread_cond_t CondID;

void *Blocker(
  void *argument
)
{

  uint32_t end_time;
  struct   sched_param param;
  int      policy;
  int      status;

  status = pthread_mutex_lock(&MutexID);
  rtems_test_assert( status == 0 );
  status = pthread_getschedparam(pthread_self(), &policy, &param);
  rtems_test_assert( status == 0 );
  param.sched_priority = sched_get_priority_max(policy) - 1;
  status = pthread_setschedparam(pthread_self(), policy, &param);
  /* Thread blocks, unlocks mutex, waits for CondID to be signaled */
  pthread_cond_wait(&CondID,&MutexID);

  /* Once signaled, this thread preempts POSIX_Init thread */
  end_time = benchmark_timer_read();
  put_time(
    "pthread_cond_signal: thread waiting preempt",
    end_time,
    1,
    0,
    0
  );
  TEST_END();
  rtems_test_exit( 0 );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        status;
  pthread_t  threadId;

  TEST_BEGIN();

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  
  status = pthread_mutex_init(&MutexID, NULL);
  rtems_test_assert( status == 0 );

  status = pthread_cond_init(&CondID, NULL); /* Create condition variable */
  rtems_test_assert( status == 0 );

  /*
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
  /* let other thread run */

  /* Other thread is blocked and waiting on condition to be signaled */
  benchmark_timer_initialize();
  status = pthread_cond_signal(&CondID);
  rtems_test_assert ( status == 0 );
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
