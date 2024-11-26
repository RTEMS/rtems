/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <sched.h>

#define CONFIGURE_INIT
#include "system.h"
#include <errno.h>
#include "pritime.h"

const char rtems_test_name[] = "PSX 9";

static int get_current_prio( void )
{
  rtems_status_code sc;
  rtems_task_priority prio;
  int max;

  sc = rtems_task_set_priority( RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &prio );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  max = sched_get_priority_max( SCHED_FIFO );

  return max + 1 - (int) prio;
}

static void *mutex_lock_task(void *arg)
{
  pthread_mutex_t *mtx;
  int              eno;

  mtx = arg;

  eno = pthread_mutex_lock( mtx );
  rtems_test_assert( eno == 0 );

  sched_yield();

  eno = pthread_mutex_unlock( mtx );
  rtems_test_assert( eno == 0 );

  return NULL;
}

static void test_destroy_locked_mutex(void)
{
  pthread_mutex_t mtx;
  pthread_t       th;
  int             eno;

  eno = pthread_mutex_init( &mtx, NULL );
  rtems_test_assert( eno == 0 );

  eno = pthread_create( &th, NULL, mutex_lock_task, &mtx );
  rtems_test_assert( eno == 0 );

  sched_yield();

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == EBUSY );

  sched_yield();

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_join( th, NULL );
  rtems_test_assert( eno == 0 );
}

void *POSIX_Init(
  void *argument
)
{
  int                  status;
  int                  passes;
  int                  schedpolicy;
  int                  priority;
  struct sched_param   schedparam;
  char                 buffer[ 80 ];
  pthread_mutexattr_t  attr;
  time_t               start;
  int                  ceiling_priority;
  int                  high_priority;
  int                  low_priority;
  rtems_id             scheduler_id;
  rtems_task_priority  rtems_priority;
  rtems_status_code    sc;

  if (argument != NULL ) {
    /* We end up here due to the rtems_task_restart() below */

    ceiling_priority = sched_get_priority_max( SCHED_SPORADIC );
    low_priority = ceiling_priority - 2;
    rtems_test_assert( get_current_prio() == low_priority );

    while ( get_current_prio() == low_priority ) {
      /* Be busy until sched_ss_repl_period elapses */
    }

    rtems_test_assert( get_current_prio() == 2 );

    TEST_END();
    rtems_test_exit( 0 );
  }

  TEST_BEGIN();

  ceiling_priority = sched_get_priority_max( SCHED_SPORADIC );
  high_priority = ceiling_priority - 1;
  low_priority = ceiling_priority - 2;

  test_destroy_locked_mutex();

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", Init_id );

  /* try to use this thread as a sporadic server */

  puts( "Init: pthread_getschedparam - SUCCESSFUL" );
  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  priority = schedparam.sched_priority;
  sprintf( buffer, " - current priority = %d", priority );
  print_current_time( "Init: ", buffer );

  schedparam.sched_ss_repl_period.tv_sec = 2;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 1;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  schedparam.sched_priority = high_priority;
  schedparam.sched_ss_low_priority = low_priority;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  sprintf( buffer, " - new priority = %d", get_current_prio() );
  print_current_time( "Init: ", buffer );

  /* go into a loop consuming CPU time to watch our priority change */

  for ( passes=0 ; passes <= 3 ; ) {
    int current_priority;

    current_priority = get_current_prio();

    if ( priority != current_priority ) {
      priority = current_priority;
      sprintf( buffer, " - new priority = %d", priority );
      print_current_time( "Init: ", buffer );
      passes++;
    }
  }

  /* now see if this works if we are holding a priority ceiling mutex */

  empty_line();

  status = pthread_getschedparam( pthread_self(), &schedpolicy, &schedparam );
  rtems_test_assert( !status );

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = 500000000;  /* 1/2 second */
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = 250000000;    /* 1/4 second */

  schedparam.sched_priority = high_priority;
  schedparam.sched_ss_low_priority = low_priority;

  puts( "Init: pthread_setschedparam - SUCCESSFUL (sporadic server)" );
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: Initializing mutex attributes for priority ceiling" );
  status = pthread_mutexattr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_PROTECT );
  rtems_test_assert( !status );

  status = pthread_mutexattr_setprioceiling( &attr, ceiling_priority );
  rtems_test_assert( !status );

  puts( "Init: Creating a mutex" );
  status = pthread_mutex_init( &Mutex_id, &attr );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );

  sprintf( buffer, " - new priority = %d", get_current_prio() );
  print_current_time( "Init: ", buffer );

  start = time( &start );

  puts( "Init: Go into low priority and lock/unlock ceiling mutex" );

  while ( get_current_prio() == low_priority ) {
    /* Be busy until sched_ss_repl_period elapses */
  }

  rtems_test_assert( get_current_prio() == high_priority );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d %s\n", status, strerror(status) );
  rtems_test_assert( !status );
  rtems_test_assert( get_current_prio() == ceiling_priority );

  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d %s\n", status, strerror(status) );
  rtems_test_assert( !status );
  rtems_test_assert( get_current_prio() == high_priority );

  puts( "Init: Go into high priority and lock/unlock ceiling mutex" );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  status = pthread_mutex_lock( &Mutex_id );
  if ( status )
    printf( "status = %d %s\n", status, strerror(status) );
  rtems_test_assert( !status );
  rtems_test_assert( get_current_prio() == ceiling_priority );

  status = pthread_mutex_unlock( &Mutex_id );
  if ( status )
    printf( "status = %d\n", status );
  rtems_test_assert( !status );
  rtems_test_assert( get_current_prio() == low_priority );

  status = pthread_mutex_destroy( &Mutex_id );
  rtems_test_assert( status == 0 );

  puts( "Init: Go into low priority and change scheduler parameters" );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  schedparam.sched_priority = ceiling_priority;
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == ceiling_priority );

  puts( "Init: Go into low priority and set POSIX priority" );

  schedparam.sched_priority = high_priority;
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == high_priority );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  status = pthread_setschedprio( pthread_self(), ceiling_priority );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == low_priority );

  while ( get_current_prio() == low_priority ) {
    /* Be busy until sched_ss_repl_period elapses */
  }

  rtems_test_assert( get_current_prio() == ceiling_priority );

  puts( "Init: Go into low priority and set RTEMS priority" );

  sc = rtems_task_get_scheduler( RTEMS_SELF, &scheduler_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  sc = rtems_scheduler_map_priority_from_posix(
    scheduler_id,
    ceiling_priority,
    &rtems_priority
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  schedparam.sched_priority = high_priority;
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == high_priority );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  sc = rtems_task_set_priority( RTEMS_SELF, rtems_priority, &rtems_priority );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( get_current_prio() == low_priority );

  while ( get_current_prio() == low_priority ) {
    /* Be busy until sched_ss_repl_period elapses */
  }

  rtems_test_assert( get_current_prio() == ceiling_priority );

  puts( "Init: Go into low priority and set scheduler" );

  sc = rtems_scheduler_ident_by_processor( 1, &scheduler_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  schedparam.sched_priority = high_priority;
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == high_priority );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  sc = rtems_task_set_scheduler( RTEMS_SELF, scheduler_id, 1 );
  rtems_test_assert( sc == RTEMS_RESOURCE_IN_USE );
  rtems_test_assert( get_current_prio() == low_priority );

  while ( get_current_prio() == low_priority ) {
    /* Be busy until sched_ss_repl_period elapses */
  }

  rtems_test_assert( get_current_prio() == high_priority );

  puts( "Init: Go into low priority and restart task" );

  schedparam.sched_priority = high_priority;
  status = pthread_setschedparam( pthread_self(), SCHED_SPORADIC, &schedparam );
  rtems_test_assert( status == 0 );
  rtems_test_assert( get_current_prio() == high_priority );

  while ( get_current_prio() == high_priority ) {
    /* Be busy until sched_ss_init_budget elapses */
  }

  rtems_test_assert( get_current_prio() == low_priority );
  (void) rtems_task_restart( RTEMS_SELF, 1 );

  return NULL; /* just so the compiler thinks we returned something */
}
