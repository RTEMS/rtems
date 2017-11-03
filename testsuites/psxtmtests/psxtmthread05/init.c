/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *  COPYRIGHT (c) 2013.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timesys.h>
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMTHREAD 05";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_pthread_create(void);
void benchmark_pthread_setschedparam(void);
void *test_thread(void *argument);

void benchmark_pthread_setschedparam(void)
{
  int status;
  int policy;
  struct sched_param param;
  pthread_t thread_ID;

  status = pthread_create(&thread_ID, NULL, test_thread, NULL);
  rtems_test_assert( status == 0 );

  /* make test_thread equal to POSIX_Init() */
  pthread_getschedparam(pthread_self(), &policy, &param);

  pthread_setschedparam(thread_ID, policy, &param);
  /* At this point, we've switched to test_thread */

  /* Back from test_thread, switch to test_thread again */
  param.sched_priority = sched_get_priority_min(policy);

  benchmark_timer_initialize();
  //lower own priority to minimun, scheduler forces an involuntary context switch
  pthread_setschedparam(pthread_self(), policy, &param);
}

void *test_thread(
  void *argument
)
{
  uint32_t end_time;

  sched_yield();

  end_time = benchmark_timer_read();
  put_time(
    "pthread_setschedparam: lower own priority preempt",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

  TEST_END();
  rtems_test_exit(0);
  //Empty thread used in pthread_create().
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();
  benchmark_pthread_setschedparam();

  rtems_test_assert( 1 );
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
