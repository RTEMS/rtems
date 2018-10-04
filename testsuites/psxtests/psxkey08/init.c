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

#include <rtems/libcsupport.h>

const char rtems_test_name[] = "PSXKEY 8";

static pthread_key_t Key;
static int created_task_count, setted_task_count, got_task_count;
static int all_thread_created;
static rtems_id sema1, sema2;
static rtems_name name1, name2;

static rtems_task test_task(rtems_task_argument arg)
{
  rtems_status_code sc;
  const void *value_p;
  const void *value_p2;

  value_p = &sc;
  sc = pthread_setspecific( Key, value_p );
  rtems_test_assert( !sc );
  ++setted_task_count;
  sc = rtems_semaphore_release( sema1 );

  /* Blocked until all tasks have been created */
  sc = rtems_semaphore_obtain( sema2 , RTEMS_WAIT, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_semaphore_release( sema2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  value_p2 = pthread_getspecific( Key );
  rtems_test_assert( value_p == value_p2 );
  ++got_task_count;

  rtems_task_exit();
}

static rtems_task Init(rtems_task_argument arg)
{
  rtems_status_code        sc;
  int                      eno;
  rtems_resource_snapshot  snapshot;
  uintptr_t                max_free_size = 13 * RTEMS_MINIMUM_STACK_SIZE;
  void                    *greedy;

  all_thread_created = 0;

  TEST_BEGIN();
  rtems_resource_snapshot_take(&snapshot);

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
  eno = pthread_key_create( &Key, NULL );
  rtems_test_assert( eno == 0 );

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

  /* Ready all created tasks one by one to let them check the key data.*/
  puts( "Init - release semaphore 2 - OK" );
  sc = rtems_semaphore_release( sema2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  printf( "Init - %d Tasks have been got key data - OK\n", got_task_count );
  rtems_test_assert( created_task_count == got_task_count );
  puts( "Init - pthread Key delete - OK" );
  eno = pthread_key_delete( Key );
  rtems_test_assert( eno == 0 );

  puts( "Init - semaphore 1 delete - OK" );
  sc = rtems_semaphore_delete( sema1 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  puts( "Init - semaphore 2 delete - OK" );
  sc = rtems_semaphore_delete( sema2 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));
  TEST_END();
  exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 14
#define CONFIGURE_MAXIMUM_SEMAPHORES 2
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_INIT_TASK_INITIAL_MODES \
  (RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | RTEMS_INTERRUPT_LEVEL(0))

#define CONFIGURE_INIT_TASK_PRIORITY 4
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
/* end of file */
