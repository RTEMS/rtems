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

#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>

const char rtems_test_name[] = "PSXTMCOND 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_create_cond_var(void);
void benchmark_destroy_cond_var(void);

pthread_cond_t mycondvar;

void benchmark_create_cond_var(void)
{
  uint32_t end_time;
  int      status;

  benchmark_timer_initialize();
  status = pthread_cond_init(&mycondvar, NULL);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_cond_init: only case",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_destroy_cond_var(void)
{
  uint32_t end_time;
  int      status;

  benchmark_timer_initialize();
  status = pthread_cond_destroy(&mycondvar);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_cond_destroy: only case",
    end_time,
    1,
    0,
    0
  );
}

void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();

  benchmark_create_cond_var();
  benchmark_destroy_cond_var();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

/* configure an instance of the condition variable created and destroyed */

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
