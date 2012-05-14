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

#include <coverhd.h>
#include <pthread.h>
#include <sched.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <rtems/timerdrv.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

pthread_rwlock_t     rwlock;

void *Blocker(
  void *argument
)
{
  (void) pthread_rwlock_wrlock(&rwlock);
  /* should never return */
  rtems_test_assert( FALSE );

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int                   status;
  pthread_t             threadId;
  benchmark_timer_t end_time;
  pthread_rwlockattr_t  attr;

  puts( "\n\n*** POSIX TIME TEST PSXTMRWLOCK 06 ***" );

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  /*
   * Deliberately create the rwlock after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   */

  /* creating rwlock */
    status = pthread_rwlockattr_init( &attr );
  rtems_test_assert( status == 0 );
  status = pthread_rwlock_init( &rwlock, &attr );
  rtems_test_assert( status == 0 );

  /*
   * Ensure the rwlock is unavailable so the other threads block.
   */
  /* lock rwlock to ensure thread blocks */
  status = pthread_rwlock_wrlock(&rwlock);
  rtems_test_assert( status == 0 );

  /*
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other thread run */

  benchmark_timer_initialize();
    status = pthread_rwlock_unlock(&rwlock); /*  unlock the rwlock */
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_unlock - thread waiting, no preempt",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST PSXTMRWLOCK 06 ***" );
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_MAXIMUM_POSIX_RWLOCKS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
