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

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <coverhd.h>
#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/timerdrv.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

#define MAX_SEMS  2

sem_t           sem1;
sem_t           *n_sem1;

void *Blocker( void *argument)
{
  (void) sem_wait(&sem1);
  /* should never return */
  rtems_test_assert( FALSE );

  return NULL;
}

void *POSIX_Init(void *argument)
{
  int        status;
  pthread_t  threadId;
  benchmark_timer_t end_time;

  puts( "\n\n*** POSIX TIME TEST SEMAPHORE 04 ***" );

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  
  /*
   * Deliberately create the semaphore after the threads.  This way if the
   * threads do run before we intend, they will get an error.
   */
  status = sem_init( &sem1, 0, 1 );
  rtems_test_assert( status == 0 );

  /*
   * Ensure the semaphore is unavailable so the other threads block.
   * Lock semaphore resource to ensure thread blocks.
   */
  status = sem_wait(&sem1);
  rtems_test_assert( status == 0 );

  /*
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
    /* let other thread run */

  benchmark_timer_initialize();
    status = sem_post( &sem1 ); /* semaphore unblocking operation */
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_post - thread waiting, no preempt",
    end_time,
    1,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST SEMAPHORE 04 ***" );
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES  MAX_SEMS
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
