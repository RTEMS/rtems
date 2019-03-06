/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

const char rtems_test_name[] = "PSXTMCOND 08";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_cond_t  CondID;
pthread_mutex_t MutexID;
struct timespec sleepTime;

void *Low(
  void *argument
)
{
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
