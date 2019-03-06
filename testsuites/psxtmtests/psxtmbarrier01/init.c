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

#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"
#include <pthread.h>

const char rtems_test_name[] = "PSXTMBARRIER 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
static void benchmark_pthread_barrier_init(void);

pthread_barrier_t   barrier;

static void benchmark_pthread_barrier_init(void)
{
  benchmark_timer_t end_time;
  int                   status;
  pthread_barrierattr_t attr;

  /* initialize attr with default attributes
   * for all of the attributes defined by the implementation
   */
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  benchmark_timer_initialize();
    status = pthread_barrier_init( &barrier,&attr, 1 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrier_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_pthread_barrier_destroy(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_barrier_destroy( &barrier );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrier_destroy: only case",
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

  /* creating barrier with default properties */
  benchmark_pthread_barrier_init();
  /* destroying barrier */
  benchmark_pthread_barrier_destroy();

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
