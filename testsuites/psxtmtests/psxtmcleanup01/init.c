/*
 *  COPYRIGHT (c) 2018.
 *  Himanshu Sekhar Nayak(GCI 2018)
 *
 *  Permission to use, copy, modify, and/or distribute this software
 *  for any purpose with or without fee is hereby granted.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *  WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 *  BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 *  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 *  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 *  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <timesys.h>
#include <tmacros.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMCLEANUP01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

int status = 1;

static void routine(void *arg)
{
}

void benchmark_pthread_cleanup_push_pull(void)
{
  long end_time;
  int execute = 1;
  status = 0;

  benchmark_timer_initialize();
  pthread_cleanup_push(routine, NULL);
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  put_time(
    "pthread_cleanup_push : only case",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

  benchmark_timer_initialize();
  pthread_cleanup_pop( execute );
  end_time = benchmark_timer_read();
  rtems_test_assert( status == 0 );
  put_time(
    "pthread_cleanup_pop : only case",
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

  benchmark_pthread_cleanup_push_pull();

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
