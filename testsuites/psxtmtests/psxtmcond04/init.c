/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2013.
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

const char rtems_test_name[] = "PSXTMCOND 04";

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

  uint32_t end_time;
  struct   sched_param param;
  int      policy;
  int      status;

  status = pthread_mutex_lock(&MutexID);
  rtems_test_assert( status == 0 );
  status = pthread_getschedparam(pthread_self(), &policy, &param);
  rtems_test_assert( status == 0 );
  param.sched_priority = sched_get_priority_max(policy) - 1;
  status = pthread_setschedparam(pthread_self(), policy, &param);
  /* Thread blocks, unlocks mutex, waits for CondID to be signaled */
  pthread_cond_wait(&CondID,&MutexID);

  /* Once signaled, this thread preempts POSIX_Init thread */
  end_time = benchmark_timer_read();
  put_time(
    "pthread_cond_signal: thread waiting preempt",
    end_time,
    1,
    0,
    0
  );
  TEST_END();
  rtems_test_exit( 0 );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int        status;
  pthread_t  threadId;

  TEST_BEGIN();

  status = pthread_create( &threadId, NULL, Blocker, NULL );
  rtems_test_assert( status == 0 );
  
  status = pthread_mutex_init(&MutexID, NULL);
  rtems_test_assert( status == 0 );

  status = pthread_cond_init(&CondID, NULL); /* Create condition variable */
  rtems_test_assert( status == 0 );

  /*
   * Let the other thread start so the thread startup overhead,
   * is accounted for.  When we return, we can start the benchmark.
   */
  sched_yield();
  /* let other thread run */

  /* Other thread is blocked and waiting on condition to be signaled */
  benchmark_timer_initialize();
  status = pthread_cond_signal(&CondID);
  rtems_test_assert ( status == 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
