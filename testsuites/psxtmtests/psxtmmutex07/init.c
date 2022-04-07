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

const char rtems_test_name[] = "PSXTMMUTEX 07";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void test_mutex_setprioceiling(void);
void test_mutex_getprioceiling(void);

pthread_mutex_t MutexId;

void test_mutex_setprioceiling(void)
{
  benchmark_timer_t end_time;
  int  status;
  int  old_ceiling;

  benchmark_timer_initialize();
    status = pthread_mutex_setprioceiling( &MutexId, 5, &old_ceiling );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutex_setprioceiling: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

void test_mutex_getprioceiling(void)
{
  benchmark_timer_t end_time;
  int  status;
  int  current_ceiling;

  benchmark_timer_initialize();
    status = pthread_mutex_getprioceiling( &MutexId, &current_ceiling );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutex_getprioceiling: only case",
    end_time,
    1,        /* Only executed once */
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
  /* create mutex*/
  status = pthread_mutex_init( &MutexId, NULL );
  rtems_test_assert( status == 0 );

  test_mutex_getprioceiling();
  test_mutex_setprioceiling();
  test_mutex_getprioceiling();

  /* destroy mutex */
  status = pthread_mutex_destroy( &MutexId );
  rtems_test_assert( status == 0 );

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
