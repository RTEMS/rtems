/*
 * Thread Test Program
 * 
 *  - test of POSIX's pthread_init() function from rtemstask Init()
 *
 *     ott@linux.thai.net
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#ifdef __rtems__
#include <rtems.h>
/* configuration information */

#define CONFIGURE_INIT

#include <unistd.h>
#include <errno.h>
#include <sched.h>

#include <bsp.h> /* for device driver prototypes */
#include <pmacros.h>

rtems_task Init( rtems_task_argument argument);

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             3
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_EXTRA_TASK_STACKS         (3 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_MAXIMUM_POSIX_THREADS 5
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 5
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 5

#include <console.h>
#include <confdefs.h>

#endif /* __rtems__ */

void countTaskDeferred() {
  int i=0;
  int type,state;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &type);
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &state);
  while (1) {
    printf("countTaskDeferred: elapsed time (second): %2d\n", i++ );
    sleep(1);
    pthread_testcancel();
    }
}

void countTaskAsync() {
  int i=0;
  int type,state;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &type);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &state);
  while (1) {
    printf("countTaskAsync: elapsed time (second): %2d\n", i++ );
    sleep(1);
    }
}

#ifdef __linux__
int main(){
#else
  rtems_task Init( rtems_task_argument ignored ) {
#endif

  pthread_t count;
  int taskparameter = 0;

  puts( "\n\n*** POSIX CANCEL TEST ***" );

  /* Start countTask deferred */
  {
    int task_ret;
    task_ret = pthread_create(&count, NULL, (void *) countTaskDeferred, (void *) &taskparameter);
    if (task_ret) {
      perror("pthread_create: countTask");
      rtems_test_exit(EXIT_FAILURE);
    }
    /* sleep for 5 seconds, then cancel it */
    sleep(5);
    pthread_cancel(count);
    pthread_join(count,NULL);
  }

  /* Start countTask asynchronous */
  {
    int task_ret;
    task_ret = pthread_create(&count, NULL, (void *) countTaskAsync, (void *) &taskparameter);
    if (task_ret) {
      perror("pthread_create: countTask");
      rtems_test_exit(EXIT_FAILURE);
    }
    /* sleep for 5 seconds, then cancel it */
    sleep(5);
    pthread_cancel(count);
    pthread_join(count,NULL);
  }


  puts( "*** END OF POSIX CANCEL TEST ***" );

#ifdef __linux__
  return 0;
#else
  rtems_test_exit(EXIT_SUCCESS);
#endif
}

