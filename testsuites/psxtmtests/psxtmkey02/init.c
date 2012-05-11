/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http:www.rtems.com/license/LICENSE.
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
void benchmark_pthread_setspecific(void *value_p);
void benchmark_pthread_getspecific( void *expected );

pthread_key_t Key;
int           Value1;

void benchmark_pthread_setspecific( void *value_p )
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_setspecific( Key, value_p );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_setspecific",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

void benchmark_pthread_getspecific( void *expected )
{
  benchmark_timer_t end_time;
  void *value_p;

  benchmark_timer_initialize();
    value_p = pthread_getspecific( Key );
  end_time = benchmark_timer_read();
  rtems_test_assert( value_p == expected );

  put_time(
    "pthread_getspecific",
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
  int  status;

  puts( "\n\n*** POSIX TIME TEST PSXTMKEY02 ***" );

  /* create the key */
  status = pthread_key_create( &Key, NULL );
  rtems_test_assert( status == 0 );

  benchmark_pthread_getspecific( NULL );
  benchmark_pthread_setspecific( &Value1 );
  benchmark_pthread_getspecific( &Value1 );

  /* destroy the key */
  status = pthread_key_delete( Key );
  rtems_test_assert( status == 0 );

  puts( "*** END OF POSIX TIME TEST PSXTMKEY02 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
