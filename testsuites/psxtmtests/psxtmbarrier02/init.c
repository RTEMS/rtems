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

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timesys.h>
#include <tmacros.h>
#include <rtems/btimer.h>
#include "test_support.h"

#include <pthread.h>
#include <sched.h>

const char rtems_test_name[] = "PSXTMBARRIER 02";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_barrier_t     barrier;

void *Low(
  void *argument
)
{
  (void) argument;

  benchmark_timer_t end_time;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

  end_time = benchmark_timer_read();

  put_time(
    "pthread_barrier_wait: blocking",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  TEST_END();

  rtems_test_exit( 0 );
  return NULL;
}

void *Middle(
  void *argument
)
{
  (void) argument;

  /*
   * Now we have finished the thread startup overhead,
   * so let other threads run.  When we return, we can
   * finish the benchmark.
   */
  sched_yield();
    /* let other threads run */

    (void) pthread_barrier_wait( &barrier );

  /* should never return */
  rtems_test_assert( FALSE );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int        i;
  int        status;
  pthread_t  threadId;
  pthread_barrierattr_t attr;

  TEST_BEGIN();

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }

  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /*
   * Create the barrier with default attributes and deliberately
   * create it after the Threads so if they run too early, they
   * wull fail with an error.
   */
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  /*
   * Set threshold on count higher than number of threads so all will
   * block.
   */
  status = pthread_barrier_init( &barrier,&attr, OPERATION_COUNT + 2 );
  rtems_test_assert( status == 0 );

  /*
   * Let other threads run through their initialization
   */
  sched_yield();

  /*
   * Start the timer and start the blocking barrier wait chain through
   * all the other tasks.
   */
  benchmark_timer_initialize();
    /* blocking barrier call */
    status = pthread_barrier_wait( &barrier );
  rtems_test_assert( status == 0 );
  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
