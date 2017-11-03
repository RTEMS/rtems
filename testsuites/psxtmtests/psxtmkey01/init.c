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
#include <rtems/btimer.h>
#include <errno.h>
#include <pthread.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMKEY 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

pthread_key_t Key;

static void benchmark_pthread_key_create(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_key_create(&Key, NULL);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_key_create: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_key_delete(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_key_delete(Key);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_key_delete: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

void *POSIX_Init(void *argument)
{

  TEST_BEGIN();

  /*key creation, using NULL destructor*/
  benchmark_pthread_key_create();

  /* key deletion*/
  benchmark_pthread_key_delete();

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
