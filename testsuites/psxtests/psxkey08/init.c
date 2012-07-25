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

#include <stdio.h>
#include <rtems.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

pthread_key_t Key;
int created_task_count, setted_task_count, got_task_count;
int all_thread_created;
pthread_mutex_t mutex1, mutex2;
pthread_cond_t create_condition_var, set_condition_var;

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument arg);
rtems_task test_task(rtems_task_argument arg);

rtems_task test_task(rtems_task_argument arg)
{
  int sc;
  int *value_p, *value_p2;
  
  value_p = malloc( sizeof( int ) );
  //printf( "Test_Task%d  - Key pthread_setspecific - OK\n", (int)rtems_task_self() );
  sc = pthread_setspecific( Key, value_p );
  rtems_test_assert( !sc );
  pthread_mutex_lock( &mutex1 );
  ++setted_task_count;
  pthread_cond_signal( &set_condition_var );
  pthread_mutex_unlock( &mutex1 );

  /**
   * blocked untill all threads have been created.
   */
  pthread_mutex_lock( &mutex2 );
  while( !all_thread_created )
    pthread_cond_wait( &create_condition_var, &mutex2 );
  pthread_mutex_unlock( &mutex2 );
  
  //printf( "Test_Task%d  - Key pthread_getspecific - OK\n", (int)rtems_task_self() );

  value_p2 = pthread_getspecific( Key );
  rtems_test_assert( value_p == value_p2 );
  ++got_task_count;

  pthread_exit( 0 );
}

rtems_task Init(rtems_task_argument arg)
{
  rtems_status_code status;
  int              sc;
  rtems_id        *task_id_p;
  all_thread_created = 0;

  puts( "\n\n*** TEST KEY 08 ***" );

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
      task_id_p = malloc( sizeof( rtems_id ) );
      rtems_test_assert( task_id_p );
      sc = rtems_task_create(
	     rtems_build_name('T','A',created_task_count, ' '),
	     1,
	     RTEMS_MINIMUM_STACK_SIZE,
	     RTEMS_DEFAULT_MODES,
	     RTEMS_DEFAULT_ATTRIBUTES,
	     task_id_p
	     );
      rtems_test_assert( (sc == RTEMS_SUCCESSFUL) || (sc == RTEMS_UNSATISFIED) );
      /**
       * when return is RTEMS_TOO_MANY or RTEMS_UNSATISFIED, there is not
       * enough source to create task.
       */
      if ( (sc == RTEMS_TOO_MANY) || (sc == RTEMS_UNSATISFIED) )
	{
	  pthread_mutex_unlock( &mutex1 );
	  break;
	}
      ++created_task_count;
      pthread_mutex_lock( &mutex1 );
      sc = rtems_task_start( *task_id_p,  test_task, 0 );
      rtems_test_assert( sc == RTEMS_SUCCESSFUL );
      while( created_task_count > setted_task_count )
	pthread_cond_wait( &set_condition_var, &mutex1 );
      pthread_mutex_unlock( &mutex1 );
    }
  
  printf( "Init - %d tasks have been created - OK\n", created_task_count );
  printf( "Init - %d tasks have been setted key data - OK\n", setted_task_count );
  rtems_test_assert( created_task_count == setted_task_count );
  
  /* unblock all created pthread to let them set key data.*/
  pthread_mutex_lock( &mutex2 );
  all_thread_created = 1;
  pthread_cond_broadcast( &create_condition_var );
  pthread_mutex_unlock( &mutex2 );

  puts( "Init - sleep to yield processor - OK" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  printf( "Init - %d Tasks have been got key data - OK\n", got_task_count );
  rtems_test_assert( created_task_count == got_task_count );
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
  
  puts( "*** END OF TEST KEY 08***" );
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(5)
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES              2
#define CONFIGURE_MAXIMUM_POSIX_KEYS     1
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES  2

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
