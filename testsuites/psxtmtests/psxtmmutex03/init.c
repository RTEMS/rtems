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

#include <timesys.h>
#include <rtems/btimer.h>
#include <errno.h>
#include <pthread.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMMUTEX 03";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_mutex_lock_available(void);
void benchmark_mutex_unlock_no_threads_waiting(void);
void benchmark_mutex_trylock_available(void);
void benchmark_mutex_trylock_not_available(void);
void benchmark_mutex_timedlock_available(void);

pthread_mutex_t MutexId;

void benchmark_mutex_lock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_lock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_lock: available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_unlock_no_threads_waiting(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_unlock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_unlock: no threads waiting",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_trylock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_trylock( &MutexId );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_trylock: available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_trylock_not_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_trylock( &MutexId );
    /*
     * it has to return a negative value
     * because it try to lock a not available mutex
     * so the assert call is make with status instead !status
     */
  end_time = benchmark_timer_read();
  rtems_test_assert( status );

  put_time(
    "pthread_mutex_trylock: not available",
    end_time,
    1,
    0,
    0
  );
}

void benchmark_mutex_timedlock_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = pthread_mutex_timedlock( &MutexId, 0 );
  end_time = benchmark_timer_read();
  rtems_test_assert( !status );

  put_time(
    "pthread_mutex_timedlock: available",
    end_time,
    1,
    0,
    0
  );
}

void *POSIX_Init(
  void *argument
)
{
  int  status;

  TEST_BEGIN();

  /*
   * Create the single Mutex used in all the test case
   */
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert( !status );

  /*
   * Now invoke subroutines to time each test case
   * get the goal depends of its order
   */
  benchmark_mutex_lock_available();
  benchmark_mutex_unlock_no_threads_waiting();
  benchmark_mutex_trylock_available();
  benchmark_mutex_trylock_not_available();
  benchmark_mutex_unlock_no_threads_waiting();
  benchmark_mutex_timedlock_available();
  benchmark_mutex_unlock_no_threads_waiting();


  /*
   *  Destroy the mutex used in the tests
   */
  status = pthread_mutex_destroy( &MutexId );
  rtems_test_assert( !status );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
