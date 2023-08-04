/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  Copyright (c) 2012 Zhongwei Yao.
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include "tmacros.h"
#include "pmacros.h"

const char rtems_test_name[] = "PSXKEY 7";

#define INITIAL_TASK_COUNT 10

#define ADDITIONAL_TASK_COUNT 13

static pthread_key_t Key;
static int created_thread_count, setted_thread_count, got_thread_count;
static int all_thread_created;
static pthread_mutex_t mutex1, mutex2;
static pthread_cond_t create_condition_var, set_condition_var;

static rtems_task Test_Thread(rtems_task_argument argument)
{
  int sc;
  int *value_p, *value_p2;

  value_p = malloc( sizeof( int ) );
  rtems_test_assert(value_p != NULL);

  *value_p = 123;
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

  value_p2 = pthread_getspecific( Key );
  rtems_test_assert( value_p == value_p2 );
  ++got_thread_count;

  rtems_task_exit();
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code  rc;
  int                sc;
  struct timespec    delay_request;
  uintptr_t          max_free_size =
                       ADDITIONAL_TASK_COUNT * RTEMS_MINIMUM_STACK_SIZE;
  void              *greedy;

  all_thread_created = 0;

  TEST_BEGIN();

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

  /* Reduce workspace size if necessary to shorten test time */
  greedy = rtems_workspace_greedy_allocate( &max_free_size, 1 );

  for ( ; ; ) {
    rtems_id task_id;

    pthread_mutex_lock( &mutex1 );

    rc = rtems_task_create(
      rtems_build_name( 'T', 'E', 'S', 'T' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    rtems_test_assert(
      ( rc == RTEMS_SUCCESSFUL ) || ( rc == RTEMS_UNSATISFIED )
        || ( rc == RTEMS_TOO_MANY )
    );

    if ( rc == RTEMS_SUCCESSFUL ) {
      rc = rtems_task_start( task_id, Test_Thread, 0 );
      rtems_test_assert( rc == RTEMS_SUCCESSFUL );
    }

    /**
     * check if return is not successful, it means RTEMS Workspace RAM
     * have been exhausted.
     */
    if ( rc != RTEMS_SUCCESSFUL ) {
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

  rtems_workspace_greedy_free( greedy );

  printf(
    "Init - %d pthreads have been created - OK\n"
    "Init - %d pthreads have been setted key data - OK\n",
    created_thread_count,
    setted_thread_count
  );
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

  printf(
    "Init - %d pthreads have been got key data - OK\n",
    got_thread_count
  );
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

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_TASKS rtems_resource_unlimited(INITIAL_TASK_COUNT)
#define CONFIGURE_MAXIMUM_POSIX_KEYS 1
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
  (INITIAL_TASK_COUNT + ADDITIONAL_TASK_COUNT)
#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
