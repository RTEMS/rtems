/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"
#include <pthread.h>
#include <sched.h>
#include <rtems/btimer.h>

#define N 5

const char rtems_test_name[] = "PSXTMCOND 06";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Blocker(void *argument);

pthread_mutex_t MutexID;
pthread_cond_t CondID;

void *Blocker(
  void *argument
)
{
  (void) argument;

  int status;

  status = pthread_mutex_lock(&MutexID);
  rtems_test_assert( status == 0 );

  /* Unlock mutex, block, wait for CondID to be signaled */
  pthread_cond_wait(&CondID,&MutexID);

 /* should never return */
  rtems_test_assert( FALSE );

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  uint32_t            end_time;
  int                 status;
  int                 i;
  pthread_t           threadId;
  pthread_attr_t      attr;
  struct sched_param  param;
  int                 policy;

  TEST_BEGIN();

  /* Setup variables */
  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  status = pthread_mutex_init(&MutexID, NULL);
  rtems_test_assert( status == 0 );
  status = pthread_cond_init(&CondID, NULL);
  rtems_test_assert( status == 0 );

  /* Setup so threads are created with a high enough priority to preempt
   * as they get created.
   */
  status = pthread_attr_init( &attr );
  rtems_test_assert( status == 0 );
  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( status == 0 );
  status = pthread_attr_setschedpolicy( &attr, SCHED_FIFO );
  rtems_test_assert( status == 0 );
  param.sched_priority = sched_get_priority_max(SCHED_FIFO) / 2;
  status = pthread_attr_setschedparam( &attr, &param );
  rtems_test_assert( status == 0 );

  for ( i=0 ; i < N ; i++) {
    /* Threads will preempt as they are created, start up, and block */
    status = pthread_create(&threadId, &attr, Blocker, NULL);
    rtems_test_assert( status == 0 );
  }

  /* Now that all the threads have been created, adjust our priority
   * so we don't get preempted on broadcast.
   */
  status = pthread_getschedparam(pthread_self(), &policy, &param);
  rtems_test_assert( status == 0 );
  param.sched_priority = sched_get_priority_max(policy) - 1;
  status = pthread_setschedparam(pthread_self(), policy, &param);
  rtems_test_assert( status == 0 );

  benchmark_timer_initialize();
  status = pthread_cond_broadcast(&CondID);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_cond_broadcast: threads waiting no preempt",
    end_time,
    1,
    0,
    0
  );

  TEST_END();
  rtems_test_exit( 0 );

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS  2 + N
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
