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
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMCOND 02";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_signal(void);

pthread_cond_t CondID;

void benchmark_signal(void)
{
  uint32_t end_time;
  int      status;

  benchmark_timer_initialize();

  status = pthread_cond_signal(&CondID);

  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_cond_signal: no threads waiting",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();

  pthread_cond_init(&CondID, NULL);

  benchmark_signal();

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
