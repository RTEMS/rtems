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

const char rtems_test_name[] = "PSXTMRWLOCK 07";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

pthread_rwlock_t     rwlock;

void *Low(
  void *argument
)
{
  int      status;
  benchmark_timer_t end_time;

  /* write locking */
    status = pthread_rwlock_wrlock(&rwlock);
  end_time = benchmark_timer_read();

  rtems_test_assert( status == 0 );

  put_time(
    "pthread_rwlock_unlock: thread waiting preempt",
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
  int status;

  /* write locking */
  status =  pthread_rwlock_wrlock(&rwlock);
  rtems_test_assert( status == 0 );

    /* thread switch occurs */

    status = pthread_rwlock_unlock(&rwlock); /*  unlock the rwlock */
  rtems_test_assert( status == 0 );
    /* thread switch occurs */

  /* should never return */
  rtems_test_assert( FALSE );
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int                 i;
  int                 status;
  pthread_t           threadId;
  pthread_attr_t      attr;
  struct sched_param  param;
  pthread_rwlockattr_t rw_attr;

  TEST_BEGIN();

  /*
   * Deliberately create the lock BEFORE the threads.  This way the
   * threads should preempt this thread and block as they are created.
   */
  status = pthread_rwlockattr_init( &rw_attr );
  rtems_test_assert( status == 0 );
  status = pthread_rwlock_init( &rwlock, &rw_attr );
  rtems_test_assert( status == 0 );

  /*
   * Obtain the lock so the threads will block.
   */
  status = pthread_rwlock_wrlock(&rwlock);
  rtems_test_assert( status == 0 );

  /*
   * Now lower our priority
   */
  status = pthread_attr_init( &attr );
  rtems_test_assert( status == 0 );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( status == 0 );

  status = pthread_attr_setschedpolicy( &attr, SCHED_RR );
  rtems_test_assert( status == 0 );

  param.sched_priority = 2;
  status = pthread_attr_setschedparam( &attr, &param );
  rtems_test_assert( status == 0 );

  /*
   * And create rest of threads as more important than we are.  They
   * will preempt us as they are created and block.
   */
  for ( i=0 ; i < OPERATION_COUNT ; i++ ) {

    param.sched_priority = 3 + i;
    status = pthread_attr_setschedparam( &attr, &param );
    rtems_test_assert( status == 0 );

    status = pthread_create(
      &threadId,
      &attr,
      (i == OPERATION_COUNT - 1) ? Low : Middle,
      NULL
    );
    rtems_test_assert( status == 0 );
  }

  /*
   * Now start the timer which will be stopped in Low
   * Release the lock.  Threads unblock and preempt.
   */
  benchmark_timer_initialize();

    status = pthread_rwlock_unlock(&rwlock);
      /* thread switch occurs */

  /* should never return */
  rtems_test_assert( FALSE );
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
