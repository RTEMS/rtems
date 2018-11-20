/*
 *  COPYRIGHT (c) 2018.
 *  Himanshu Sekhar Nayak( GCI 2018 )
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include <tmacros.h>
#include <sched.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMMUTEXATTR01";

/* forward declarations to avoid warnings */
static void *POSIX_Init(void *argument);

static pthread_mutexattr_t attr;

static void benchmark_create_pthread_mutexattr(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_init( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setprioceiling(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setprioceiling( &attr, SCHED_FIFO);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setprioceiling: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getprioceiling(void)
{
  long end_time;
  int  status;
  int prioceiling;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getprioceiling( &attr, &prioceiling);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( prioceiling == SCHED_FIFO);

  put_time(
    "pthread_mutexattr_getprioceiling: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setprotocol(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setprotocol: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getprotocol(void)
{
  long end_time;
  int  status;
  int protocol;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getprotocol( &attr, &protocol );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( protocol == PTHREAD_PRIO_INHERIT );

  put_time(
    "pthread_mutexattr_getprotocol: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setpshared(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getpshared(void)
{
  long end_time;
  int  status;
  int pshared;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getpshared( &attr, &pshared );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( pshared == PTHREAD_PROCESS_PRIVATE );

  put_time(
    "pthread_mutexattr_getpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_settype(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_DEFAULT );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_settype: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_gettype(void)
{
  long end_time;
  int  status;
  int type;

  benchmark_timer_initialize();
  status = pthread_mutexattr_gettype( &attr, &type );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( type == PTHREAD_MUTEX_DEFAULT );

  put_time(
    "pthread_mutexattr_gettype: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_destroy_pthread_mutexattr(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_destroy( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_destroy: only case",
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

  benchmark_create_pthread_mutexattr();
  benchmark_pthread_mutexattr_setprioceiling();
  benchmark_pthread_mutexattr_getprioceiling();
  benchmark_pthread_mutexattr_setprotocol();
  benchmark_pthread_mutexattr_getprotocol();
  benchmark_pthread_mutexattr_setpshared();
  benchmark_pthread_mutexattr_getpshared();
  benchmark_pthread_mutexattr_settype();
  benchmark_pthread_mutexattr_gettype();
  benchmark_destroy_pthread_mutexattr();

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
