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

#include <timesys.h>
#include <pthread.h>
#include <rtems/timerdrv.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void benchmark_pthread_create(void);
void *thread(void *argument);

void benchmark_pthread_create(void)
{
  long end_time;
  int  status;
  pthread_t thread_ID;

  benchmark_timer_initialize();
  
  status = pthread_create(&thread_ID, NULL, thread, NULL);
  rtems_test_assert( status == 0 );
 
  end_time = benchmark_timer_read();

  rtems_test_assert( status == 0 );

  put_time(
    "pthread_create - preempt",
    end_time,
    1,        /* Only executed once */
    0,
    0
  );

}

void *thread(
  void *argument
)
{
  //Empty thread used in pthread_create().
  return NULL;
}

void *POSIX_Init(
  void *argument
)
{

  puts( "\n\n*** POSIX TIME TEST PSXTMTHREAD02 ***" );

  benchmark_pthread_create();

  puts( "*** END OF POSIX TIME TEST PSXTMTHREAD02 ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
