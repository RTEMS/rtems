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

const char rtems_test_name[] = "PSXTMTHREAD 04";

/* forward declarations to avoid warnings */
void benchmark_pthread_setschedparam(void);
void benchmark_pthread_getschedparam(void);
void *POSIX_Init(void *argument);

void benchmark_pthread_getschedparam(void)
{
  uint32_t            end_time;
  int                 status;
  int                 policy;
  struct sched_param  param;

  benchmark_timer_initialize();
  status = pthread_getschedparam( pthread_self(), &policy, &param );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_getschedparam: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

void benchmark_pthread_setschedparam(void)
{
  uint32_t            end_time;
  int                 status;
  int                 policy;
  struct sched_param  param;

  status = pthread_getschedparam( pthread_self(), &policy, &param );
  rtems_test_assert( status == 0 );

  /* Arbitrary priority, no other threads to preempt us so it doesn't matter. */
  param.sched_priority = 5;
  benchmark_timer_initialize();
  status = pthread_setschedparam( pthread_self(), policy, &param );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
     "pthread_setschedparam: no thread switch",
     end_time,
     1,       /* Only executed once */
     0,
     0
  );
}

void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();

  benchmark_pthread_getschedparam();
  benchmark_pthread_setschedparam();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
