/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, Himanshu Sekhar Nayak
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

#include <pthread.h>
#include <tmacros.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMBARRIERATTR01";

/* forward declarations to avoid warnings */
static void *POSIX_Init(void *argument);

static pthread_barrierattr_t attr;

static void benchmark_create_barrierattr(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_init( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_barrierattr_setpshared(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_setpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_barrierattr_getpshared(void)
{
  benchmark_timer_t end_time;
  int               status;
  int               pshared;

  benchmark_timer_initialize();
  status = pthread_barrierattr_getpshared( &attr, &pshared );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( pshared == PTHREAD_PROCESS_SHARED );

  put_time(
    "pthread_barrierattr_getpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_destroy_barrierattr(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_barrierattr_destroy( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_barrierattr_destroy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void *POSIX_Init(
  void *argument
)
{

  TEST_BEGIN();

  benchmark_create_barrierattr();
  benchmark_barrierattr_setpshared();
  benchmark_barrierattr_getpshared();
  benchmark_destroy_barrierattr();

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
