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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include <tmacros.h>
#include <sched.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMMUTEXATTR01";

/* forward declarations to avoid warnings */
static void *POSIX_Init(void *argument);

static pthread_mutexattr_t attr;

static void benchmark_create_pthread_mutexattr(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_init( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setprioceiling(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setprioceiling( &attr, SCHED_FIFO);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setprioceiling: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getprioceiling(void)
{
  benchmark_timer_t end_time;
  int               status;
  int               prioceiling;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getprioceiling( &attr, &prioceiling);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( prioceiling == SCHED_FIFO);

  put_time(
    "pthread_mutexattr_getprioceiling: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setprotocol(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setprotocol( &attr, PTHREAD_PRIO_INHERIT );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setprotocol: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getprotocol(void)
{
  benchmark_timer_t end_time;
  int               status;
  int               protocol;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getprotocol( &attr, &protocol );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( protocol == PTHREAD_PRIO_INHERIT );

  put_time(
    "pthread_mutexattr_getprotocol: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_setpshared(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_setpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_getpshared(void)
{
  benchmark_timer_t end_time;
  int               status;
  int               pshared;

  benchmark_timer_initialize();
  status = pthread_mutexattr_getpshared( &attr, &pshared );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( pshared == PTHREAD_PROCESS_PRIVATE );

  put_time(
    "pthread_mutexattr_getpshared: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_settype(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_DEFAULT );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_settype: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_pthread_mutexattr_gettype(void)
{
  benchmark_timer_t end_time;
  int               status;
  int               type;

  benchmark_timer_initialize();
  status = pthread_mutexattr_gettype( &attr, &type );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  rtems_test_assert( type == PTHREAD_MUTEX_DEFAULT );

  put_time(
    "pthread_mutexattr_gettype: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

static void benchmark_destroy_pthread_mutexattr(void)
{
  benchmark_timer_t end_time;
  int               status;

  benchmark_timer_initialize();
  status = pthread_mutexattr_destroy( &attr );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "pthread_mutexattr_destroy: only case",
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

  TEST_BEGIN();

  benchmark_create_pthread_mutexattr();
  benchmark_pthread_mutexattr_setprioceiling();
  benchmark_pthread_mutexattr_getprioceiling();
  benchmark_pthread_mutexattr_setprotocol();
  benchmark_pthread_mutexattr_getprotocol();
  benchmark_pthread_mutexattr_setpshared();
  benchmark_pthread_mutexattr_getpshared();
  benchmark_pthread_mutexattr_settype();
  benchmark_pthread_mutexattr_gettype();
  benchmark_destroy_pthread_mutexattr();

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
