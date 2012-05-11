/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#if defined(__rtems__)
  #include <rtems.h>
  #include <bsp.h>
  #include <pmacros.h>
#endif

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void countTask_cancel_handler(void *ignored);
void *countTaskDeferred(void *ignored);
void *countTaskAsync(void *ignored);

volatile bool countTask_handler;

void countTask_cancel_handler(void *ignored)
{
  countTask_handler = true;
}

void *countTaskDeferred(void *ignored)
{
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

void *countTaskAsync(void *ignored)
{
  int i=0;
  int type,state;

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &type);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &state);
  pthread_cleanup_push(countTask_cancel_handler, NULL);
  while (1) {
    printf("countTaskAsync: elapsed time (second): %2d\n", i++ );
    sleep(1);
  }
  countTask_handler = false;
  pthread_cleanup_pop(1);
  if ( countTask_handler == false ){
    puts("countTask_cancel_handler not executed");
    rtems_test_exit(0);
  }
}

#if defined(__rtems__)
  void *POSIX_Init(void *ignored)
#else
  int main(int argc, char **argv)
#endif
{
  pthread_t task;
  int       taskparameter = 0;
  int       sc;
  int       old;

  puts( "\n\n*** POSIX CANCEL TEST ***" );

  /* generate some error conditions */
  puts( "Init - pthread_setcancelstate - NULL oldstate - EINVAL" );
  sc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  fatal_posix_service_status( sc, EINVAL, "cancel state EINVAL" );

  puts( "Init - pthread_setcancelstate - bad state - EINVAL" );
  sc = pthread_setcancelstate(12, &old);
  fatal_posix_service_status( sc, EINVAL, "cancel state EINVAL" );

  puts( "Init - pthread_setcanceltype - NULL oldtype - EINVAL" );
  sc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  fatal_posix_service_status( sc, EINVAL, "cancel type EINVAL" );

  puts( "Init - pthread_setcanceltype - bad type - EINVAL" );
  sc = pthread_setcanceltype(12, &old);
  fatal_posix_service_status( sc, EINVAL, "cancel type EINVAL" );

  puts( "Init - pthread_cancel - bad ID - EINVAL" );
  sc = pthread_cancel(0x100);
  fatal_posix_service_status( sc, EINVAL, "cancel bad Id" );

  /* Start countTask deferred */
  {
    sc = pthread_create(&task, NULL, countTaskDeferred, &taskparameter);
    if (sc) {
      perror("pthread_create: countTask");
      rtems_test_exit(EXIT_FAILURE);
    }
    /* sleep for 5 seconds, then cancel it */
    sleep(5);
    pthread_cancel(task);
    pthread_join(task, NULL);
  }

  /* Start countTask asynchronous */
  {
    sc = pthread_create(&task, NULL, countTaskAsync, &taskparameter);
    if (sc) {
      perror("pthread_create: countTask");
      rtems_test_exit(EXIT_FAILURE);
    }
    /* sleep for 5 seconds, then cancel it */
    sleep(5);
    pthread_cancel(task);
    pthread_join(task, NULL);
  }


  puts( "*** END OF POSIX CANCEL TEST ***" );

  #if defined(__rtems__)
    rtems_test_exit(EXIT_SUCCESS);
    return NULL;
  #else
    return 0;
  #endif
}

/* configuration information */
#if defined(__rtems__)

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif /* __rtems__ */

