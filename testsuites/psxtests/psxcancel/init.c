/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
#include <semaphore.h>

#if defined(__rtems__)
  #include <rtems.h>
  #include <rtems/libcsupport.h>
  #include <pmacros.h>
#endif

const char rtems_test_name[] = "PSXCANCEL";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#if defined(__rtems__)
static rtems_resource_snapshot initialSnapshot;
#endif

static volatile bool countTask_handler;

static sem_t masterSem;

static sem_t workerSem;

static void countTask_cancel_handler(void *ignored)
{
  countTask_handler = true;
}

static void *countTaskDeferred(void *ignored)
{
  int i=0;
  int type,state;
  int sc;

  sc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &type);
  fatal_posix_service_status( sc, 0, "cancel state deferred" );
  rtems_test_assert( type == PTHREAD_CANCEL_ENABLE );
  sc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &state);
  fatal_posix_service_status( sc, 0, "cancel type deferred" );
  rtems_test_assert( state == PTHREAD_CANCEL_DEFERRED );
  while (1) {
    printf("countTaskDeferred: elapsed time (second): %2d\n", i++ );
    sleep(1);
    pthread_testcancel();
  }
}

static void *countTaskAsync(void *ignored)
{
  int i=0;
  int type,state;
  int sc;

  sc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &type);
  fatal_posix_service_status( sc, 0, "cancel state async" );
  rtems_test_assert( type == PTHREAD_CANCEL_ENABLE );
  sc = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &state);
  fatal_posix_service_status( sc, 0, "cancel type async" );
  rtems_test_assert( state == PTHREAD_CANCEL_DEFERRED );
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

static void *taskAsyncAndDetached(void *ignored)
{
  int sc;

  sc = pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
  fatal_posix_service_status( sc, 0, "cancel type taskAsyncAndDetached" );

  sc = sem_post( &workerSem );
  rtems_test_assert( sc == 0 );

  sc = sem_wait( &masterSem );
  rtems_test_assert( sc == 0 );

  rtems_test_assert( 0 );
}

static void *taskSelfDetach(void *ignored)
{
  int sc;

  sc = sem_post( &workerSem );
  rtems_test_assert( sc == 0 );

  sleep( 1 );

  sc = pthread_detach( pthread_self() );
  fatal_posix_service_status( sc, 0, "detach taskSelfDetach" );

  pthread_exit( (void *) 123 );
}

static void resourceSnapshotInit( void )
{
#if defined(__rtems__)
  rtems_resource_snapshot_take( &initialSnapshot );
#endif
}

static void resourceSnapshotCheck( void )
{
#if defined(__rtems__)
  rtems_test_assert( rtems_resource_snapshot_check( &initialSnapshot ) );
#endif
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
  void     *exit_value;

  TEST_BEGIN();

  sc = sem_init( &masterSem, 0, 0 );
  rtems_test_assert( sc == 0 );

  sc = sem_init( &workerSem, 0, 0 );
  rtems_test_assert( sc == 0 );

  resourceSnapshotInit();

  /* generate some error conditions */
  puts( "Init - pthread_setcancelstate - NULL oldstate" );
  sc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  fatal_posix_service_status( sc, 0, "cancel state NULL" );

  puts( "Init - pthread_setcancelstate - bad state - EINVAL" );
  sc = pthread_setcancelstate(12, &old);
  fatal_posix_service_status( sc, EINVAL, "cancel state EINVAL" );

  puts( "Init - pthread_setcanceltype - NULL oldtype" );
  sc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  fatal_posix_service_status( sc, 0, "cancel type NULL" );

  puts( "Init - pthread_setcanceltype - bad type - EINVAL" );
  sc = pthread_setcanceltype(12, &old);
  fatal_posix_service_status( sc, EINVAL, "cancel type EINVAL" );

  puts( "Init - pthread_cancel - bad ID - ESRCH" );
  sc = pthread_cancel(0x100);
  fatal_posix_service_status( sc, ESRCH, "cancel bad Id" );

  resourceSnapshotCheck();

  /* Test resource reclamation due to pthread_detach() */

  sc = pthread_create( &task, NULL, taskAsyncAndDetached, NULL );
  fatal_posix_service_status( sc, 0, "create taskAsyncAndDetached" );

  sc = sem_wait( &workerSem );
  rtems_test_assert( sc == 0 );

  sc = pthread_cancel( task );
  fatal_posix_service_status( sc, 0, "cancel taskAsyncAndDetached" );

  sc = pthread_detach( task );
  fatal_posix_service_status( sc, 0, "detach taskAsyncAndDetached" );

  sched_yield();

  sc = pthread_join( task, &exit_value );
  fatal_posix_service_status( sc, ESRCH, "join taskAsyncAndDetached" );

  resourceSnapshotCheck();

  /* Test pthread_detach() after pthread_join() */

  sc = pthread_create( &task, NULL, taskSelfDetach, NULL );
  fatal_posix_service_status( sc, 0, "create taskSelfDetach" );

  sc = sem_wait( &workerSem );
  rtems_test_assert( sc == 0 );

  sc = pthread_join( task, &exit_value );
  fatal_posix_service_status( sc, 0, "join taskSelfDetach" );
  rtems_test_assert( exit_value == (void *) 123 );

  resourceSnapshotCheck();

  /* Start countTask deferred */
  {
    sc = pthread_create(&task, NULL, countTaskDeferred, &taskparameter);
    if (sc) {
      perror("pthread_create: countTask");
      rtems_test_exit(EXIT_FAILURE);
    }
    /* sleep for 5 seconds, then cancel it */
    sleep(5);
    sc = pthread_cancel(task);
    fatal_posix_service_status( sc, 0, "cancel deferred" );
    sc = pthread_join(task, NULL);
    fatal_posix_service_status( sc, 0, "join deferred" );
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
    sc = pthread_cancel(task);
    fatal_posix_service_status( sc, 0, "cancel async" );
    sc = pthread_join(task, NULL);
    fatal_posix_service_status( sc, 0, "join async" );
  }

  resourceSnapshotCheck();

  TEST_END();

  #if defined(__rtems__)
    rtems_test_exit(EXIT_SUCCESS);
    return NULL;
  #else
    return 0;
  #endif
}

/* configuration information */
#if defined(__rtems__)

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

#endif /* __rtems__ */

