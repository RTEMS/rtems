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

#include <sched.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <tmacros.h>
#include <pmacros.h>
#include "test_support.h"

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

#define fatal_posix_sem( _ptr, _msg ) \
  if ( (_ptr != SEM_FAILED) ) { \
    check_dispatch_disable_level( 0 ); \
    printf( "\n%s FAILED -- expected (-1) got (%p - %d/%s)\n", \
	    (_msg), _ptr, errno, strerror(errno) ); \
    FLUSH_OUTPUT(); \
    rtems_test_exit( -1 ); \
  }


#define MAX_SEMS  10

void *POSIX_Init(
  void *argument
)
{
  int             status;
  int             value;
  int             i;
  sem_t           sems[MAX_SEMS];
  sem_t           sem2;
  sem_t           *n_sem1;
  sem_t           *n_sem2;
  struct timespec waittime;
  char            failure_msg[80];

  puts( "\n\n*** POSIX SEMAPHORE MANAGER TEST 1 ***" );

  puts( "Init: sem_init - UNSUCCESSFUL (EINVAL)" );
  status = sem_init(NULL, 0, 1);
  fatal_posix_service_status( status, -1, "sem_init error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_init errorno EINVAL" );

  puts( "Init: sem_init - SUCCESSFUL" );
  for (i = 0; i < MAX_SEMS; i++) {
    status = sem_init(&sems[i], 0, i);
    sprintf(failure_msg, "sem_init %d", i );
    fatal_posix_service_status( status, 0, failure_msg);
  }
  puts( "Init: sem_init - UNSUCCESSFUL (ENOSPC)" );
  status = sem_init(&sem2, 0, 1);
  fatal_posix_service_status( status, -1, "sem_init error return status");
  fatal_posix_service_status( errno, ENOSPC, "sem_init errorno ENOSPC" );

  puts( "Init: sem_init - UNSUCCESSFUL (ENOSYS -- pshared not supported)" );
  status = sem_init(&sem2, 1, 1);
  fatal_posix_service_status( status, -1, "sem_init error return status");
  fatal_posix_service_status( errno, ENOSYS, "sem_init errno set to ENOSYS");

  puts( "Init: sem_getvalue - SUCCESSFUL ");
  for (i = 0; i < MAX_SEMS; i++) {
    status = sem_getvalue(&sems[i], &value);
    sprintf( failure_msg, "sem_getvalue %d", i );
    fatal_posix_service_status( status, 0, failure_msg );
    fatal_posix_service_status( value, i, "sem_getvalue correct value" );
  }
  puts( "Init: sem_getvalue - UNSUCCESSFUL ");
  status = sem_getvalue(&sem2, &value);
  fatal_posix_service_status( status, -1, "sem_getvalue error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_getvalue errno EINVAL");

  puts( "Init: sem_destroy - SUCCESSFUL" );
  status = sem_destroy(&sems[0]);
  fatal_posix_service_status( status, 0, "sem_destroy semaphore 0");

  puts( "Init: sem_destroy - UNSUCCESSFUL (EINVAL)" );
  status = sem_destroy(&sem2);
  fatal_posix_service_status( status, -1, "sem_destroy error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_destroy errno EINVAL");

  puts( "Init: sem_wait - SUCCESSFUL" );
  status = sem_wait(&sems[1]);
  fatal_posix_service_status( status, 0, "sem_wait semaphore 1");
  /* sem[1].count = 0 */

  puts( "Init: sem_wait - UNSUCCESSFUL (EINVAL)" );
  status = sem_wait(&sem2);
  fatal_posix_service_status( status, -1, "sem_wait error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_wait errno EINVAL");

  puts( "Init: sem_post - SUCCESSFUL" );
  status = sem_post(&sems[1]);
  fatal_posix_service_status( status, 0, "sem_post semaphore 1");
  /* sem[1].count = 1 */

  puts( "Init: sem_wait - SUCCESSFUL (after a sem_post)" );
  status = sem_wait(&sems[1]);
  fatal_posix_service_status( status, 0, "sem_wait semaphore 1");
  /* sem[1].count = 0 */

  puts( "Init: sem_trywait - SUCCESSFUL" );
  status = sem_trywait(&sems[2]);
  fatal_posix_service_status( status, 0, "sem_trywait semaphore 2");
  /* sem[2].count = 1 */

  puts( "Init: sem_trywait - UNSUCCESSFUL (EAGAIN)" );
  status = sem_trywait(&sems[1]);
  fatal_posix_service_status( status, -1, "sem_trywait error return status");
  fatal_posix_service_status( errno, EAGAIN, "sem_trywait errno EAGAIN");
  /* sem[1].count = 0 */

  puts( "Init: sem_trywait - UNSUCCESSFUL (EINVAL)" );
  status = sem_trywait(&sem2);
  fatal_posix_service_status( status, -1, "sem_trywait error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_trywait errno EINVAL");

#if 0
  status = sem_post(&sems[2]);
  fatal_posix_service_status( status, 0, "sem_post semaphore 2");
  /* sem[2].count = 2 */
#else
  /* sem[2].count = 1 */
#endif

  puts( "Init: sem_timedwait - SUCCESSFUL" );
  waittime.tv_sec = time(NULL) + 1;
  waittime.tv_nsec = 100;
  status = sem_timedwait(&sems[2], &waittime);
  fatal_posix_service_status( status, 0, "sem_timedwait semaphore 2");
  /* sem[2].count = 0 */

  puts( "Init: sem_timedwait - UNSUCCESSFUL (ETIMEDOUT)" );
  status = sem_timedwait(&sems[2], &waittime);
  fatal_posix_service_status( status, -1, "sem_timedwait error return status");
  fatal_posix_service_status(
    errno, ETIMEDOUT, "sem_timedwait errno ETIMEDOUT");

  /*
   * To do this case, we must be blocking when we want the semaphore.
   * POSIX doesn't want you to check the error if you can get the resource.
   */

#if 1
  puts( "Init: sem_timedwait - UNSUCCESSFUL (EINVAL) -- skipping" );
#else
  puts( "Init: sem_timedwait - UNSUCCESSFUL (EINVAL)" );
  waittime.tv_sec = 0;
  waittime.tv_nsec = 0x7FFFFFFF;
  status = sem_timedwait(&sems[2], &waittime);
  fatal_posix_service_status( status, -1, "sem_timedwait error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_init errno EINVAL");
#endif

  puts( "Init: sem_post - UNSUCCESSFUL (EINVAL)" );
  status = sem_post(&sem2);
  fatal_posix_service_status( status, -1, "sem_post error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_post errno EINVAL");

  puts( "Init: sem_destroy - SUCCESSFUL" );
  for (i = 1; i < MAX_SEMS; i++) {
    status = sem_destroy(&sems[i]);
    sprintf( failure_msg, "sem_destroy %d", i );
    fatal_posix_service_status( status, 0, failure_msg );
  }

  /* Modes are currently unsupported */

  /*
   * Validate all sem_open return paths.
   */

  puts( "Init: sem_open - UNSUCCESSFUL (ENAMETOOLONG)" );
  n_sem1 = sem_open(Get_Too_Long_Name(), O_CREAT, 0777, 1 );
  fatal_posix_sem( n_sem1, "sem_open error return status" );
  fatal_posix_service_status(
    errno, ENAMETOOLONG, "sem_open errorno ENAMETOOLONG" );

  puts( "Init: sem_open - sem1 SUCCESSFUL" );
  n_sem1 = sem_open( "sem1",O_CREAT, 0777, 1 );
  rtems_test_assert( n_sem1 != SEM_FAILED );

  puts( "Init: sem_destroy - named sem1 - EINVAL" );
  status = sem_destroy(n_sem1);
  fatal_posix_service_status( status, -1, "sem_destroy named semaphore");
  fatal_posix_service_status( errno, EINVAL,  "sem_destroy named semaphore");

  puts( "Init: sem_open - Create an Existing sem (EEXIST)" );
  n_sem2 = sem_open("sem1", O_CREAT | O_EXCL, 0777, 1);
  fatal_posix_sem( n_sem2, "sem_open error return status" );
  fatal_posix_service_status( errno, EEXIST,  "sem_open errno EEXIST");

  puts( "Init: sem_open - Open new sem without create flag (ENOENT)" );
  n_sem2 = sem_open("sem3", O_EXCL, 0777, 1);
  fatal_posix_sem( n_sem2, "sem_open error return status" );
  fatal_posix_service_status( errno, ENOENT,  "sem_open errno EEXIST");

  /*
   * XXX - Could not hit the following errors:
   *   E_POSIX_Semaphore_Create_support only fails if
   *     ENOSYS - When semaphore is shared between processes.
   *     ENOSPC - When out of memory.
   */

  /*
   * Validate we can wait on a semaphore opened with sem_open.
   */

  puts( "Init: sem_wait on sem1" );
  status = sem_wait(n_sem1);
  fatal_posix_service_status( status, 0, "sem_wait opened semaphore");

  /*
   * Validate a second open returns the same semaphore.
   */

  puts( "Init: sem_open - Open an existing sem ( same id )" );
  n_sem2 = sem_open("sem1", 0 );
  rtems_test_assert( n_sem2 == n_sem1 );

  /*
   * Unlink the semaphore, then verify an open of the same name produces a
   * different semaphore.
   */

  puts( "Init: sem_unlink - sem1 SUCCESSFUL" );
  status = sem_unlink( "sem1" );
  fatal_posix_service_status( status, 0, "sem_unlink locked semaphore");

  puts( "Init: sem_open - Reopen sem1 SUCCESSFUL with a different id" );
  n_sem2 = sem_open( "sem1", O_CREAT | O_EXCL, 0777, 1);
  rtems_test_assert( n_sem2 != SEM_FAILED );
  rtems_test_assert( n_sem2 != n_sem1 );

  /*
   * Validate we can call close on a semaphore opened with sem_open.
   */

  puts( "Init: sem_close (1) - SUCCESSFUL" );
  status = sem_close( n_sem1 );
  fatal_posix_service_status( status, 0, "sem_close semaphore");

  /*
   * Validate it n_sem2 (the last open for sem1 name can be
   * correctly closed and unlinked.
   */

  puts( "Init: sem_close (2) - SUCCESSFUL" );
  status = sem_close( n_sem2 );
  fatal_posix_service_status( status, 0, "sem_close semaphore");

  puts( "Init: sem_unlink - sem1 (2) SUCCESSFUL" );
  status = sem_unlink( "sem1" );
  fatal_posix_service_status( status, 0, "sem_unlink locked semaphore");

  puts( "Init: sem_close - UNSUCCESSFUL (EINVAL)" );
  status = sem_close(n_sem2);
  fatal_posix_service_status( status, -1, "sem_close error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_close errno EINVAL");

  puts( "Init: sem_unlink - UNSUCCESSFUL (ENOENT)" );
  status = sem_unlink("sem1");
  fatal_posix_service_status( status, -1, "sem_unlink error return status");
  fatal_posix_service_status( errno, ENOENT, "sem_close errno EINVAL");


  /*
   * Validate we can unlink (2)
   */

  puts( "Init: sem_unlink (NULL) - EINVAL" );
  status = sem_unlink( NULL );
  fatal_posix_service_status( status, -1, "sem_unlink error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_unlink errno value");

  puts( "Init: sem_unlink (\"\") - EINVAL" );
  status = sem_unlink( "" );
  fatal_posix_service_status( status, -1, "sem_unlink error return status");
  fatal_posix_service_status( errno, EINVAL, "sem_unlink errno value");

  /*
   * XXX - Cant' create location OBJECTS_ERROR or OBJECTS_REMOTE.
   *       sem_close and sem_unlink.
   */

  puts( "Init: sem_unlink - UNSUCCESSFUL (ENOENT)" );
  status = sem_unlink("sem2");
  fatal_posix_service_status( status, -1, "sem_unlink error return status");
  fatal_posix_service_status( errno, ENOENT, "sem_unlink errno ENOENT");
  rtems_test_assert( (status == -1) && (errno == ENOENT) );


  /* Try adding in unlinking before closing... (can we still open?) */

  puts( "*** END OF POSIX SEMAPHORE MANAGER TEST 1 ***" );
  rtems_test_exit(0);

  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES  MAX_SEMS

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        (RTEMS_MINIMUM_STACK_SIZE * 4)

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
