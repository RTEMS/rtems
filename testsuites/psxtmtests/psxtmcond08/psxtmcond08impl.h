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

#if   defined(USE_WAIT)
  #define TEST_NUMBER "08"
  #define TEST_CASE "pthread_cond_wait: blocking"
#elif defined(USE_TIMEDWAIT_WITH_VALUE)
  #define TEST_NUMBER "09"
  #define TEST_CASE "pthread_cond_timedwait: blocking"
#elif defined(USE_TIMEDWAIT_WAIT_VALUE_IN_PAST)
  #define TEST_NUMBER "10"
  #define TEST_CASE "pthread_cond_timedwait: time in past error"
#elif defined(USE_CLOCKWAIT_WITH_VALUE)
  #define TEST_NUMBER "11"
  #define TEST_CASE "pthread_cond_clockwait: blocking"
#elif defined(USE_CLOCKWAIT_WAIT_VALUE_IN_PAST)
  #define TEST_NUMBER "12"
  #define TEST_CASE "pthread_cond_clockwait: time in past error"
#else
  #error "How am I being compiled?"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sched.h>
#include <timesys.h>
#include <tmacros.h>
#include <rtems/btimer.h>
#include "test_support.h"

#include <pthread.h>

const char rtems_test_name[] = "PSXTMCOND " TEST_NUMBER;

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_cond_t  CondID;
pthread_mutex_t MutexID;
struct timespec sleepTime;
clockid_t clock_id;

void *Low(
  void *argument
)
{
  (void) argument;

  uint32_t end_time;

  end_time = benchmark_timer_read();

  put_time(
    TEST_CASE,
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

  int             rc;

  rc = pthread_mutex_lock(&MutexID);
  rtems_test_assert( rc == 0 );

  /* block and switch to another task here */

  #if   defined(USE_WAIT)
    rc = pthread_cond_wait( &CondID, &MutexID );
    rtems_test_assert( rc == 0 );

  #elif defined(USE_TIMEDWAIT_WITH_VALUE)
    /* adjust sleepTime to get something obviously in the future */
    ++sleepTime.tv_sec;

    rc = pthread_cond_timedwait( &CondID, &MutexID, &sleepTime );
    rtems_test_assert( rc == 0 );

  #elif defined(USE_TIMEDWAIT_WAIT_VALUE_IN_PAST)
    {
      /* override sleepTime with something obviously in the past */
      sleepTime.tv_sec = 0;
      sleepTime.tv_nsec = 5;

      /* this does all the work of timedwait but immediately returns */
      rc = pthread_cond_timedwait( &CondID, &MutexID, &sleepTime );
      rtems_test_assert(rc == ETIMEDOUT);
      benchmark_timer_read();
    }
  #elif defined(USE_CLOCKWAIT_WITH_VALUE)
    /* adjust sleepTime to get something obviously in the future */
    ++sleepTime.tv_sec;

    rc = pthread_cond_clockwait( &CondID, &MutexID, clock_id, &sleepTime );
    rtems_test_assert( rc == 0 );

  #elif defined(USE_CLOCKWAIT_WAIT_VALUE_IN_PAST)
    {
      /* override sleepTime with something obviously in the past */
      sleepTime.tv_sec = 0;
      sleepTime.tv_nsec = 5;

      rc = pthread_cond_clockwait( &CondID, &MutexID, clock_id, &sleepTime );
      rtems_test_assert(rc == ETIMEDOUT);
      benchmark_timer_read();
    }
  #endif


  pthread_mutex_unlock(&MutexID);
  #if defined(USE_TIMEDWAIT_WAIT_VALUE_IN_PAST)
    /*
     * In this case, unlock does not switch to another thread. so we need
     * to explicitly yield. If we do not yield, then we will measure the
     * time required to do an implicit pthread_exit() which is undesirable
     * from a measurement viewpoint.
     */
    sched_yield();
  #endif
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int             i;
  int             status;
  pthread_t       threadId;
  int             rc;
  struct timeval  tp;

  TEST_BEGIN();

  rc =  gettimeofday(&tp, NULL);
  rtems_test_assert( rc == 0 );

  /* Convert from timeval to timespec */
  sleepTime.tv_sec  = tp.tv_sec;
  sleepTime.tv_nsec = tp.tv_usec * 1000;

  clock_id = CLOCK_MONOTONIC;

  rc = pthread_cond_init(&CondID, NULL);
  rtems_test_assert( rc == 0 );

  rc = pthread_mutex_init(&MutexID, NULL);
  rtems_test_assert( rc == 0 );

  rc = pthread_mutex_lock(&MutexID);
  rtems_test_assert( rc == 0 );

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }

  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /* start the timer and switch through all the other tasks */
  benchmark_timer_initialize();

  rc = pthread_mutex_unlock(&MutexID);
  rtems_test_assert( rc == 0 );

  /* Should never return. */
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

