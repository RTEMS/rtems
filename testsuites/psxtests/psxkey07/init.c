/*
 *  Copyright (c) 2012 Zhongwei Yao.
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

#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Test_Thread(void *argument);

pthread_key_t Key;
int created_thread_count, setted_thread_count, got_thread_count;
int all_thread_created;
pthread_mutex_t mutex1, mutex2;
pthread_cond_t create_condition_var, set_condition_var;

void *Test_Thread(
  void *argument
)
{
  int sc;
  int *value_p, *value_p2;

  value_p = malloc( sizeof( int ) );
  //printf( "Test_Thread%d  - Key pthread_setspecific - OK\n", (int)pthread_self() );
  sc = pthread_setspecific( Key, value_p );
  rtems_test_assert( !sc );
  pthread_mutex_lock( &mutex1 );
  ++setted_thread_count;
  pthread_cond_signal( &set_condition_var );
  pthread_mutex_unlock( &mutex1 );

  /**
   * blocked untill all threads have been created.
   */
  pthread_mutex_lock( &mutex2 );
  while( !all_thread_created )
    pthread_cond_wait( &create_condition_var, &mutex2 );
  pthread_mutex_unlock( &mutex2 );

  //printf( "Test_Thread%d  - Key pthread_getspecific - OK\n", (int)pthread_self() );
  value_p2 = pthread_getspecific( Key );
  rtems_test_assert( value_p == value_p2 );
  ++got_thread_count;

  return NULL;
}

void *POSIX_Init(
  void *ignored
)
{
  pthread_t        *thread_p;
  int              sc;
  struct timespec  delay_request;
  all_thread_created = 0;

  puts( "\n\n*** TEST KEY 07 ***" );

  puts( "Init - Mutex 1 create - OK" );
  sc = pthread_mutex_init( &mutex1, NULL );
  rtems_test_assert( !sc );
  puts( "Init - Mutex 2 create - OK" );
  sc = pthread_mutex_init( &mutex2, NULL );
  rtems_test_assert( !sc );
  puts( "Init - Condition variable 1 create - OK" );
  sc = pthread_cond_init( &create_condition_var, NULL );
  rtems_test_assert( !sc );
  puts( "Init - Condition variable 2 create - OK" );
  sc = pthread_cond_init( &set_condition_var, NULL );
  rtems_test_assert( !sc );

  puts( "Init - pthread Key create - OK" );
  sc = pthread_key_create( &Key, NULL );
  rtems_test_assert( !sc );

  for( ; ; )
    {
      thread_p = malloc( sizeof( pthread_t ) );
      rtems_test_assert( thread_p );
      pthread_mutex_lock( &mutex1 );
      sc = pthread_create( thread_p, NULL, Test_Thread, NULL );
      rtems_test_assert( ( sc == 0 ) || ( sc == EAGAIN ) );
      /**
       * check if return is EAGAIN, it means RTEMS Workspace RAM
       * have been exhausted.
       */
      if ( sc == EAGAIN )
        {
          pthread_mutex_unlock( &mutex1 );
          break;
        }
      ++created_thread_count;
      /**
       * wait for test thread set key, the while loop here is used to
       * avoid suprious wakeup.
       */
      while( created_thread_count > setted_thread_count )
        pthread_cond_wait( &set_condition_var, &mutex1 );
      pthread_mutex_unlock( &mutex1 );
    }
  printf( "Init - %d pthreads have been created - OK\n", created_thread_count );
  printf( "Init - %d pthreads have been setted key data - OK\n", setted_thread_count );
  rtems_test_assert( created_thread_count == setted_thread_count );
  /* unblock all created pthread to let them set key data.*/
  pthread_mutex_lock( &mutex2 );
  all_thread_created = 1;
  pthread_cond_broadcast( &create_condition_var );
  pthread_mutex_unlock( &mutex2 );

  puts( "Init - sleep - let threads run - OK" );
  delay_request.tv_sec = 0;
  delay_request.tv_nsec = 8 * 100000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  printf( "Init - %d pthreads have been got key data - OK\n", got_thread_count );
  rtems_test_assert( created_thread_count == got_thread_count );
  puts( "Init - pthread Key delete - OK" );
  sc = pthread_key_delete( Key );
  rtems_test_assert( sc == 0 );

  puts( "Init - Mutex1 delete - OK" );
  sc = pthread_mutex_destroy( &mutex1 );
  rtems_test_assert( !sc );
  puts( "Init - Mutex2 delete - OK" );
  sc = pthread_mutex_destroy( &mutex2 );
  rtems_test_assert( !sc );
  puts( "Init - Condition variable 1 delete - OK" );
  sc = pthread_cond_destroy( &create_condition_var );
  rtems_test_assert( !sc );
  puts( "Init - Condition variable 2 delete - OK" );
  sc = pthread_cond_destroy( &set_condition_var );
  rtems_test_assert( !sc );

  puts( "*** END OF TEST KEY 07 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER


#define CONFIGURE_MAXIMUM_POSIX_THREADS  rtems_resource_unlimited(10)
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  2
#define CONFIGURE_UNIFIED_WORK_AREAS



#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
