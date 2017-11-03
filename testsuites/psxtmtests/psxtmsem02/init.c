/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
