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
#include <tmacros.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMBARRIERATTR01";

/* forward declarations to avoid warnings */
static void *POSIX_Init(void *argument);

static pthread_barrierattr_t attr;

static void benchmark_create_barrierattr(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_init( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_barrierattr_setpshared(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_setpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_barrierattr_getpshared(void)
{
  long end_time;
  int  status;
  int pshared;

  benchmark_timer_initialize();
  status = pthread_barrierattr_getpshared( &attr, &pshared );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( pshared == PTHREAD_PROCESS_SHARED );

  put_time(
    "pthread_barrierattr_getpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_destroy_barrierattr(void)
{
  long end_time;
  int  status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_destroy( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_destroy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();

  benchmark_create_barrierattr();
  benchmark_barrierattr_setpshared();
  benchmark_barrierattr_getpshared();
  benchmark_destroy_barrierattr();

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
