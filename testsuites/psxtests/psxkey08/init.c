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

#include "tmacros.h"

#include <stdio.h>
#include <rtems.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

pthread_key_t Key;
int created_task_count, setted_task_count, got_task_count;
int all_thread_created;
rtems_id sema1, sema2;
rtems_name name1, name2;

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
  ++setted_task_count;
  sc = rtems_semaphore_release( sema1 );

  /**
   * blocked untill all tasks have been created.
   */
  rtems_semaphore_obtain( sema2 , RTEMS_WAIT, 0 );

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

  puts( "Init - Semaphore 1 create - OK" );
  name1 = rtems_build_name('S', 'E', 'M', '1');
  sc = rtems_semaphore_create( name1, 0,
                               RTEMS_SIMPLE_BINARY_SEMAPHORE |
                               RTEMS_FIFO,
                               0, &sema1 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - Semaphore 2 create - OK" );
  name2 = rtems_build_name('S', 'E', 'M', '2');
  sc = rtems_semaphore_create( name2, 0,
                               RTEMS_SIMPLE_BINARY_SEMAPHORE |
                               RTEMS_FIFO,
                               0, &sema2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

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
      rtems_test_assert( (sc == RTEMS_UNSATISFIED) || (sc == RTEMS_TOO_MANY) || (sc == RTEMS_SUCCESSFUL) );
      /**
       * when return is RTEMS_TOO_MANY or RTEMS_UNSATISFIED, there is not
       * enough source to create task.
       */
      if ( (sc == RTEMS_TOO_MANY) || (sc == RTEMS_UNSATISFIED) )
        {
          break;
        }
      ++created_task_count;
      sc = rtems_task_start( *task_id_p,  test_task, 0 );
      rtems_test_assert( sc == RTEMS_SUCCESSFUL );
      sc = rtems_semaphore_obtain( sema1, RTEMS_WAIT, 0 );
      rtems_test_assert( sc == RTEMS_SUCCESSFUL );
    }

  printf( "Init - %d tasks have been created - OK\n", created_task_count );
  printf( "Init - %d tasks have been setted key data - OK\n", setted_task_count );
  rtems_test_assert( created_task_count == setted_task_count );

  /* unblock all created tasks to let them set key data.*/
  puts( "Init - flush semaphore 2 - OK" );
  sc = rtems_semaphore_flush( sema2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - sleep to yield processor - OK" );
  status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  directive_failed( status, "rtems_task_wake_after" );

  printf( "Init - %d Tasks have been got key data - OK\n", got_task_count );
  rtems_test_assert( created_task_count == got_task_count );
  puts( "Init - pthread Key delete - OK" );
  sc = pthread_key_delete( Key );
  rtems_test_assert( sc == 0 );

  puts( "Init - semaphore 1 delete - OK" );
  sc = rtems_semaphore_delete( sema1 );
  rtems_test_assert( !sc );

  puts( "Init - semaphore 2 delete - OK" );
  sc = rtems_semaphore_delete( sema2 );
  rtems_test_assert( !sc );

  puts( "*** END OF TEST KEY 08***" );
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(5)
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
