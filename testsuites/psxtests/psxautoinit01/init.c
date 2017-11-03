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

const char rtems_test_name[] = "PSXAUTOINIT 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int             sc;
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
  int             prioceiling;

  TEST_BEGIN();

  /* path using mutex get with interrupts disabled */
  puts( "Init - pthread_mutex_lock - auto initialize - OK" );
  sc = pthread_mutex_lock( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex lock OK" );

  puts( "Init - pthread_mutex_unlock - OK" );
  sc = pthread_mutex_unlock( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex unlock OK" );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex destroy OK" );

  /* path using mutex get with dispatching disabled */
  puts( "Init - pthread_mutex_getprioceiling - auto initialize - OK" );
  prioceiling = 1;
  sc = pthread_mutex_getprioceiling( &mutex2, &prioceiling );
  fatal_posix_service_status( sc, 0, "mutex getprioceiling OK" );
  rtems_test_assert( prioceiling == 0 );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mutex2 );
  fatal_posix_service_status( sc, 0, "mutex destroy OK" );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
