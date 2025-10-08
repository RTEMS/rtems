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

const char rtems_test_name[] = "PSXTMSEM 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#define MAX_SEMS  2

sem_t           sem1;
sem_t           *n_sem1;
sem_t           *n_sem2;

static void benchmark_sem_init(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_init( &sem1, 0, 1 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_init: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_destroy(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_destroy( &sem1 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_destroy: only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_open(bool report_time)
{
  benchmark_timer_t end_time;

  benchmark_timer_initialize();
    n_sem1 = sem_open( "sem1", O_CREAT, 0777, 1 );
  end_time = benchmark_timer_read();

  if ( report_time ) {
    put_time(
      "sem_open: first open O_CREAT",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  }
}

static void benchmark_sem_close(bool report_time)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_close( n_sem1 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  if ( report_time ) {
    put_time(
      "sem_close: named first/nested close",
      end_time,
      1,        /* Only executed once */
      0,
      0
    );
  }
}

static void benchmark_sem_unlink(const char *message)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_unlink( "sem1" );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    message,
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_open_second(void)
{
  benchmark_timer_t end_time;

  benchmark_timer_initialize();
    n_sem2 = sem_open( "sem1", O_EXCL, 0777, 1 );
  end_time = benchmark_timer_read();

  put_time(
    "sem_open: second open O_EXCL",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

static void benchmark_sem_close_second(void)
{
  benchmark_timer_t end_time;
  int  status;

  benchmark_timer_initialize();
    status = sem_close( n_sem2 );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );

  put_time(
    "sem_close: named second close",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

void *POSIX_Init(void *argument)
{
  (void) argument;

  TEST_BEGIN();

  /* creating unnamed semaphore */
  benchmark_sem_init();
  /* destroying unnamed semaphore */
  benchmark_sem_destroy();

  /* opening named semaphore first time o_flag = O_CREAT */
  benchmark_sem_open(true);
  /* opening named semaphore second time o_flag = O_EXCL */
  benchmark_sem_open_second();
  /* close named semaphore first time  -- does not delete */
  benchmark_sem_close(true);
  /* unlink named semaphore -- does not delete */
  benchmark_sem_unlink("sem_unlink: does not delete");
  /*  close semaphore the second time, this actually deletes it */
  benchmark_sem_close_second();

  /* recrate named semaphore first time o_flag = O_CREAT */
  benchmark_sem_open(false);
  benchmark_sem_close(false);
  benchmark_sem_unlink("sem_unlink: deletes semaphore");

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES  MAX_SEMS
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
