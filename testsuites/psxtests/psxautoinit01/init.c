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

#include "tmacros.h"
#include <pthread.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  int             sc;
  pthread_mutex_t mutex1;
  pthread_mutex_t mutex2;
  int             prioceiling;

  puts( "\n\n*** POSIX TEST -- AUTOMATIC INITIALIZAITON 01 ***" );

  /* path using mutex get with interrupts disabled */
  mutex1 = PTHREAD_MUTEX_INITIALIZER;
  mutex2 = PTHREAD_MUTEX_INITIALIZER;
  puts( "Init - pthread_mutex_lock - auto initialize - OK" );
  sc = pthread_mutex_lock( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex lock OK" );

  puts( "Init - pthread_mutex_lock - auto initialize - EINVAL" );
  sc = pthread_mutex_lock( &mutex2 );
  fatal_posix_service_status( sc, EINVAL, "mutex lock EINVAL" );

  puts( "Init - pthread_mutex_unlock - OK" );
  sc = pthread_mutex_unlock( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex unlock OK" );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex destroy OK" );

  /* path using mutex get with dispatching disabled */
  mutex1 = PTHREAD_MUTEX_INITIALIZER;
  mutex2 = PTHREAD_MUTEX_INITIALIZER;
  puts( "Init - pthread_mutex_getprioceiling - auto initialize - OK" );
  sc = pthread_mutex_getprioceiling( &mutex1, &prioceiling );
  fatal_posix_service_status( sc, 0, "mutex getprioceiling OK" );

  puts( "Init - pthread_mutex_getprioceiling - auto initialize - EINVAL" );
  sc = pthread_mutex_getprioceiling( &mutex2, &prioceiling );
  fatal_posix_service_status( sc, EINVAL, "mutex getprioceiling EINVAL" );

  puts( "Init - pthread_mutex_destroy - OK" );
  sc = pthread_mutex_destroy( &mutex1 );
  fatal_posix_service_status( sc, 0, "mutex destroy OK" );

  puts( "*** END OF POSIX TEST AUTOMATIC INITIALIZATION 01 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
