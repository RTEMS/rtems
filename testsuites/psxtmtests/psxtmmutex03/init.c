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
#include <errno.h>
#include <pthread.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_mutex_lock_available(void);
void benchmark_mutex_unlock_no_threads_waiting(void);
void benchmark_mutex_trylock_available(void);
void benchmark_mutex_trylock_not_available(void);
void benchmark_mutex_timedlock_available(void);

pthread_mutex_t MutexId;

void benchmark_mutex_lock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_lock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_lock - available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_unlock_no_threads_waiting(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_unlock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_unlock - no threads waiting",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_trylock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_trylock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_trylock - available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_trylock_not_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_trylock( &MutexId );
    /* 
     * it has to return a negative value 
     * because it try to lock a not available mutex    
     * so the assert call is make with status instead !status 
     */
  end_time = benchmark_timer_read();
  rtems_test_assert( status );

  put_time(
    "pthread_mutex_trylock - not available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_timedlock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_timedlock( &MutexId, 0 );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_timedlock - available",
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
  int  status;

  puts( "\n\n*** POSIX TIME TEST PSXTMMUTEX03 ***" );
  
  /*
   * Create the single Mutex used in all the test case
   */
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert( !status );

  /*
   * Now invoke subroutines to time each test case 
   * get the goal depends of its order
   */
  benchmark_mutex_lock_available();
  benchmark_mutex_unlock_no_threads_waiting();
  benchmark_mutex_trylock_available();
  benchmark_mutex_trylock_not_available();
  benchmark_mutex_unlock_no_threads_waiting();
  benchmark_mutex_timedlock_available();
  benchmark_mutex_unlock_no_threads_waiting();
  

  /*
   *  Destroy the mutex used in the tests
   */
  status = pthread_mutex_destroy( &MutexId );
  rtems_test_assert( !status );

  puts( "*** END OF POSIX TIME TEST PSXTMMUTEX03 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
