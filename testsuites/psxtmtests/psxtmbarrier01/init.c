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
#include <rtems/timerdrv.h>
#include "test_support.h"
#include <pthread.h>

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
    "pthread_barrier_init",
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
    "pthread_barrier_destroy",
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
  puts( "\n\n*** POSIX TIME TEST PSXTMBARRIER 01 ***" );

  /* creating barrier with default properties */
  benchmark_pthread_barrier_init();
  /* destroying barrier */
  benchmark_pthread_barrier_destroy();
  
  puts( "*** END OF POSIX TIME TEST PSXTMBARRIER 01  ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_BARRIERS    1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
