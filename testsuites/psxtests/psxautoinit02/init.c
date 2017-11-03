/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXAUTOINIT 2";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int            sc;
  pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER;
  pthread_cond_t cond5 = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  struct timespec to;

  TEST_BEGIN();

  puts( "Init - pthread_cond_broadcast - auto initialize - OK" );
  sc = pthread_cond_broadcast( &cond1 );
  fatal_posix_service_status( sc, 0, "cond broadcast OK" );

  puts( "Init - pthread_cond_signal - auto initialize - OK" );
  sc = pthread_cond_signal( &cond2 );
  fatal_posix_service_status( sc, 0, "cond signal OK" );

  puts( "Init - pthread_cond_init - auto initialize - OK" );
  sc = pthread_cond_init( &cond3, NULL );
  fatal_posix_service_status( sc, 0, "cond init OK" );

  puts( "Init - pthread_mutex_lock - OK" );
  sc = pthread_mutex_lock( &mtx );
  fatal_posix_service_status( sc, 0, "mtx lock OK" );

  puts( "Init - pthread_cond_timedwait - auto initialize - OK" );
  to.tv_sec = 1;
  to.tv_nsec = 1;
  sc = pthread_cond_timedwait( &cond4, &mtx, &to );
  fatal_posix_service_status( sc, ETIMEDOUT, "cond timedwait OK" );

  puts( "Init - pthread_mutex_unlock - OK" );
  sc = pthread_mutex_unlock( &mtx );
  fatal_posix_service_status( sc, 0, "mtx unlock OK" );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mtx );
  fatal_posix_service_status( sc, 0, "mtx destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond5 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - EINVAL" );
  sc = pthread_cond_destroy( &cond5 );
  fatal_posix_service_status( sc, EINVAL, "cond destroy EINVAL" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond4 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond3 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond2 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  puts( "Init - pthread_cond_destroy - OK" );
  sc = pthread_cond_destroy( &cond1 );
  fatal_posix_service_status( sc, 0, "cond destroy OK" );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
