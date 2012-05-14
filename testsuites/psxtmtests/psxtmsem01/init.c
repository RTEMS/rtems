/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <tmacros.h>
#include <timesys.h>
#include <rtems/timerdrv.h>
#include "test_support.h"

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
    "sem_init",
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
    "sem_destroy",
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
      "sem_open (first open 'O_CREAT')",
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
      "sem_close (named first/nested close)",
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
    "sem_open (second open 'O_EXCL')",
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
    "sem_close (named second close)",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );
}

void *POSIX_Init(void *argument)
{

  puts( "\n\n*** POSIX TIME TEST PSXTMSEM01 ***" );

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
  benchmark_sem_unlink("sem_unlink (does not delete)");
  /*  close semaphore the second time, this actually deletes it */
  benchmark_sem_close_second();

  /* recrate named semaphore first time o_flag = O_CREAT */
  benchmark_sem_open(false);
  benchmark_sem_close(false);
  benchmark_sem_unlink("sem_unlink (deletes semaphore)");

  puts( "*** END OF POSIX TIME TEST PSXTMSEM01 ***" );

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES  MAX_SEMS
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
