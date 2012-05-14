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
#include <timesys.h>
#include <rtems/timerdrv.h>
#include <pthread.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

pthread_rwlock_t     rwlock;

static void benchmark_pthread_rwlock_init(void)
{
  benchmark_timer_t end_time;
  int  status;
  pthread_rwlockattr_t attr;

  pthread_rwlockattr_init( &attr );
  benchmark_timer_initialize();
    status = pthread_rwlock_init( &rwlock, &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_init",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_rwlock_rdlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_rdlock(&rwlock);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_rdlock - available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_rwlock_unlock(int print)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_unlock(&rwlock);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  if ( print == 1 ){
    put_time(
      "pthread_rwlock_unlock - available",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  }
}

static void benchmark_pthread_rwlock_tryrdlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_tryrdlock(&rwlock);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 || status == EBUSY );
  if (status == EBUSY) {
    put_time(
      "pthread_rwlock_tryrdlock - not available",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  } else if (status == 0) {
    put_time(
      "pthread_rwlock_tryrdlock - available",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  }
}

static void benchmark_pthread_rwlock_timedrdlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_timedrdlock(&rwlock, 0);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_timedrdlock - available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_rwlock_wrlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_wrlock(&rwlock);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_wrlock - available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_rwlock_trywrlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_trywrlock(&rwlock);
  end_time = benchmark_timer_read();

  rtems_test_assert( status == 0 || status == EBUSY );
  if ( status == EBUSY ) {
    put_time(
      "pthread_rwlock_trywrlock - not available ",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  } else if ( status == 0 ) {
    put_time(
      "pthread_rwlock_trywrlock - available",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  }
}

static void benchmark_pthread_rwlock_timedwrlock(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_timedwrlock(&rwlock,0);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_timedwrlock - available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_pthread_rwlock_destroy(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_rwlock_destroy(&rwlock);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_destroy",
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

  puts( "\n\n*** POSIX TIME PSXTMRWLOCK 01  ***" );

  /* initializing rwlock */
  benchmark_pthread_rwlock_init();
  /* applying a read lock */
  benchmark_pthread_rwlock_rdlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(0);
  /* trying to apply a read lock when it is available*/
  benchmark_pthread_rwlock_tryrdlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(0);
  /* applying a timed read lock */
  benchmark_pthread_rwlock_timedrdlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(0);
  /* applying a write lock */
  benchmark_pthread_rwlock_wrlock();
  /* trying to get read lock, when is not available*/
  benchmark_pthread_rwlock_tryrdlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(0);
  /* trying to apply a write lock, when it is available*/
  benchmark_pthread_rwlock_trywrlock();
  /* trying to get write lock, when it is not available*/
  benchmark_pthread_rwlock_trywrlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(0);
  /* applying a timed write lock */
  benchmark_pthread_rwlock_timedwrlock();
  /* unlocking rwlock */
  benchmark_pthread_rwlock_unlock(1);
  /* destroying rwlock */
  benchmark_pthread_rwlock_destroy();

  puts( "*** END OF POSIX TIME PSXTMRWLOCK 01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_RWLOCKS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
