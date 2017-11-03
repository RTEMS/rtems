/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
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

const char rtems_test_name[] = "PSXTMTHREAD 02";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_pthread_create(void);
void *thread(void *argument);

void benchmark_pthread_create(void)
{
  int  status;
  pthread_t thread_ID;
  pthread_attr_t attr;
  struct sched_param param;

  status = pthread_attr_init(&attr);
  rtems_test_assert( status == 0 );

  status = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  rtems_test_assert( status == 0 );

  status = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
  rtems_test_assert( status == 0 );

  param.sched_priority = sched_get_priority_max(SCHED_FIFO) - 1;
  status = pthread_attr_setschedparam(&attr, &param);
  rtems_test_assert( status == 0 );

  /* create second thread with max priority and get preempted on creation */
  benchmark_timer_initialize();
  status = pthread_create(&thread_ID, &attr, thread, NULL);
}

void *thread(
  void *argument
)
{
  uint32_t end_time;

  end_time = benchmark_timer_read();
  put_time(
    "pthread_create: preempt",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();

  benchmark_pthread_create();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
