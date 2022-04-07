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

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <tmacros.h>
#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMSEM 02";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

sem_t           sem1;
sem_t           *n_sem1;

static void benchmark_sem_getvalue(void)
{
  benchmark_timer_t end_time;
  int  status;
  int  value;

  benchmark_timer_initialize();
    status = sem_getvalue(&sem1, &value);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_getvalue: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_wait(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_wait(&sem1);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_wait: available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_post(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_post(&sem1);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_post: no threads waiting",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_trywait_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
  status = sem_trywait(&sem1);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_trywait: available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_trywait_not_available(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_trywait(&sem1);
  end_time = benchmark_timer_read();
  /*it must be non avalible, so status should be non zero*/
  rtems_test_assert( status != 0 );

  put_time(
    "sem_trywait: not available",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

void *POSIX_Init(void *argument)
{
  int status;
  TEST_BEGIN();

  /* create the semaphore */
  status = sem_init( &sem1, 0, 1 );
  rtems_test_assert( status == 0 );

  /* obtain the actual semaphore value */
  benchmark_sem_getvalue();
  /* lock the semaphore */
  benchmark_sem_wait();
  /* unlock the semaphore */
  benchmark_sem_post();
  /* try to lock the semaphore - available */
  benchmark_sem_trywait_available();
  /* try to lock the semaphore, not available */
  benchmark_sem_trywait_not_available();

  TEST_END();

  /*Destroying the semaphore*/
  status = sem_destroy(&sem1);
  rtems_test_assert( status == 0 );

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
