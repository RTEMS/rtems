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

#include "tmacros.h"
#include <pthread.h>
#include <errno.h>

const char rtems_test_name[] = "PSXCOND 1";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *BlockingThread(void *argument);

pthread_cond_t  Condition;
pthread_mutex_t Mutex1;
pthread_mutex_t Mutex2;

void *BlockingThread(
  void *argument
)
{
  int sc;

  puts( "BlockingThread - pthread_cond_wait with mutex not locked - EPERM" );
  sc = pthread_cond_wait( &Condition, &Mutex1 );
  fatal_posix_service_status( sc, EPERM, "mutex1 not locked" );

  sc = pthread_mutex_lock( &Mutex1 );
  fatal_posix_service_status( sc, 0, "mutex1 lock" );
  
  puts( "BlockingThread - pthread_cond_wait on Mutex1 - OK" );
  sc = pthread_cond_wait( &Condition, &Mutex1 );

  printf(
    "ERROR - BlockingThread returned from pthread_cond_wait! (rc=%d/%s)\n",
    sc,
    strerror(sc)
  );
  rtems_test_exit( 0 );

  return NULL;
}

static void test_cond_auto_initialization( void )
{
  int eno;

  {
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    eno = pthread_cond_destroy( &cond );
    rtems_test_assert( eno == 0 );

    eno = pthread_cond_destroy( &cond );
    rtems_test_assert( eno == EINVAL );
  }

  {
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    eno = pthread_cond_signal( &cond );
    rtems_test_assert( eno == 0 );

    eno = pthread_cond_destroy( &cond );
    rtems_test_assert( eno == 0 );
  }

  {
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    eno = pthread_cond_broadcast( &cond );
    rtems_test_assert( eno == 0 );

    eno = pthread_cond_destroy( &cond );
    rtems_test_assert( eno == 0 );
  }
}

void *POSIX_Init(
  void *argument
)
{
  int        sc;
  pthread_t  Thread;

  TEST_BEGIN();

  test_cond_auto_initialization();

  puts( "Init - pthread_mutex_init - Mutex1 - OK" );
  sc = pthread_mutex_init( &Mutex1, NULL );
  fatal_posix_service_status( sc, 0, "mutex1 create ok" );

  puts( "Init - pthread_mutex_init - Mutex2 - OK" );
  sc = pthread_mutex_init( &Mutex2, NULL );
  fatal_posix_service_status( sc, 0, "mutex2 create ok" );

  puts( "Init - pthread_cond_init - Condition - OK" );
  sc = pthread_cond_init( &Condition, NULL );
  fatal_posix_service_status( sc, 0, "Condition create ok" );

  puts( "Init -  pthread_create - OK" );
  sc = pthread_create( &Thread, NULL, BlockingThread, NULL );
  fatal_posix_service_status( sc, 0, "Thread create ok" );

  puts( "Init - sleep to let BlockingThread run" );
  sleep(1);

  puts( "Init - pthread_cond_wait on Mutex2 - EINVAL" );
  sc = pthread_cond_wait( &Condition, &Mutex2 );
  fatal_posix_service_status( sc, EINVAL, "cond_wait EINVAL" );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
