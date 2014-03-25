/*
 *  Copyright (c) 2012 Zhongwei Yao.
 *  COPYRIGHT (c) 1989-2014.
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

#include <stdio.h>
#include <rtems.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

const char rtems_test_name[] = "PSXKEY 8";

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
  sc = pthread_setspecific( Key, value_p );
  rtems_test_assert( !sc );
  ++setted_task_count;
  sc = rtems_semaphore_release( sema1 );

  /**
   * blocked untill all tasks have been created.
   */
  rtems_semaphore_obtain( sema2 , RTEMS_WAIT, 0 );

  value_p2 = pthread_getspecific( Key );
  rtems_test_assert( value_p == value_p2 );
  ++got_task_count;

  rtems_task_delete( RTEMS_SELF );
}

rtems_task Init(rtems_task_argument arg)
{
  rtems_status_code  status;
  int                sc;
  uintptr_t          max_free_size = 13 * RTEMS_MINIMUM_STACK_SIZE;
  void              *greedy;

  all_thread_created = 0;

  TEST_BEGIN();

  puts( "Init - Semaphore 1 create - OK" );
  name1 = rtems_build_name('S', 'E', 'M', '1');
  sc = rtems_semaphore_create(
    name1, 0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_FIFO,
    0,
    &sema1
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - Semaphore 2 create - OK" );
  name2 = rtems_build_name('S', 'E', 'M', '2');
  sc = rtems_semaphore_create(
    name2,
    0,
    RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_FIFO,
    0,
    &sema2
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - pthread Key create - OK" );
  sc = pthread_key_create( &Key, NULL );
  rtems_test_assert( !sc );

  /* Reduce workspace size if necessary to shorten test time */
  greedy = rtems_workspace_greedy_allocate( &max_free_size, 1 );

  for ( ; ; ) {
    rtems_id task_id;

    sc = rtems_task_create(
      rtems_build_name('T','A',created_task_count, ' '),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert(
      (sc == RTEMS_UNSATISFIED) ||
      (sc == RTEMS_TOO_MANY) ||
      (sc == RTEMS_SUCCESSFUL)
    );

    /**
     * when return is RTEMS_TOO_MANY or RTEMS_UNSATISFIED, there is not
     * enough source to create task.
     */
    if ( (sc == RTEMS_TOO_MANY) || (sc == RTEMS_UNSATISFIED) ) {
      break;
    }
    ++created_task_count;

    sc = rtems_task_start( task_id,  test_task, 0 );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );

    sc = rtems_semaphore_obtain( sema1, RTEMS_WAIT, 0 );
    rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  }

  rtems_workspace_greedy_free( greedy );

  printf(
    "Init - %d tasks have been created - OK\n"
    "Init - %d tasks have been setted key data - OK\n",
    setted_task_count,
    created_task_count
  );
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

  TEST_END();
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(5)
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
