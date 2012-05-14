/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>


void *POSIX_Init(
  void *argument
)
{
  int                 status;
  pthread_condattr_t  attr;
  pthread_condattr_t  attr_error;
  int                 pshared;
  pthread_cond_t      cond;
  struct timespec     timeout;

  puts( "\n\n*** POSIX TEST 10 ***" );

  puts( "Init: pthread_condattr_init" );
  status = pthread_condattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_condattr_init - EINVAL (attribute invalid)" );
  status = pthread_condattr_init( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_condattr_destroy" );
  status = pthread_condattr_destroy( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_condattr_destroy - EINVAL (attribute invalid)" );
  status = pthread_condattr_destroy( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_condattr_init" );
  status = pthread_condattr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_SHARED" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( !status );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_PRIVATE" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( !status );

  status = pthread_condattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL (attribute invalid)" );

  status = pthread_condattr_setpshared( &attr, 0x7FFF );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_condattr_setpshared - EINVAL (pshared invalid)" );

  status = pthread_condattr_getpshared( &attr, &pshared );
  rtems_test_assert( !status );
  printf( "Init: pthread_condattr_getpshared - %d\n", pshared );

  status = pthread_condattr_getpshared( NULL, &pshared );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_condattr_getpshared - EINVAL (attribute invalid)" );

  puts( "Init: pthread_cond_init - NULL attr" );
  status = pthread_cond_init( &cond, NULL );
  rtems_test_assert( !status );

/* error for attribute not initialized */

  attr_error.is_initialized = FALSE;
  status = pthread_cond_init( &cond, &attr_error );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_init - EINVAL (attr not initialized)" );

  status = pthread_cond_init( &cond, NULL );
  if ( status != ENOMEM )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ENOMEM );
  puts( "Init: pthread_cond_init - ENOMEM (too many conds)" );

  puts( "Init: pthread_cond_destroy" );
  status = pthread_cond_destroy( &cond );
  rtems_test_assert( !status );

/* error for bad condition variable passed */

  status = pthread_cond_destroy( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_destroy - EINVAL (cond invalid)" );

/* initiailize the attribute for the rest of the test */

  puts( "Init: pthread_cond_init - attr" );
  status = pthread_cond_init( &Cond1_id, &attr );
  rtems_test_assert( !status );

/* signal task1 with a condition variable */

  empty_line();

  status = pthread_create( &Task_id, NULL, Task_1, NULL );
  rtems_test_assert( !status );

/* switch to task1 to allow it to wait for a condition variable */

  puts( "Init: sleep to switch to Task_1" );
  sleep( 1 );

  status = pthread_cond_destroy( &Cond1_id );
  if ( status != EBUSY )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EBUSY );
  puts( "Init: pthread_cond_destroy - EBUSY (task1 waiting)" );

  puts( "Init: pthread_cond_signal" );
  status = pthread_cond_signal( &Cond1_id );
  rtems_test_assert( !status );

  empty_line();

  status = pthread_create( &Task2_id, NULL, Task_2, NULL );
  rtems_test_assert( !status );

/* switch to task1 and task2 to allow them to wait for broadcast signal */

  puts( "Init: sleep - switch to Task_1 and Task_2" );
  sleep( 1 );

/* broadcast a condition variable to task1 and task2 */

  puts( "Init: pthread_cond_broadcast" );
  status = pthread_cond_broadcast( &Cond1_id );
  rtems_test_assert( !status );

  puts( "Init: sleep - switch to Task_1" );
  sleep( 0 );

/* timedwait case - timeout */

  status = pthread_mutex_lock( &Mutex_id );
  rtems_test_assert( !status );

/* set timeout to 3 seconds */

  status = clock_gettime( CLOCK_REALTIME, &timeout );
  rtems_test_assert( !status );
  timeout.tv_sec += 3;
  timeout.tv_nsec = 0;

  puts( "Init: pthread_cond_timedwait for 3 seconds" );
  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_timedwait - ETIMEDOUT - (mutex not acquired)" );

  status = pthread_mutex_unlock( &Mutex_id );
  rtems_test_assert( !status );

/* remaining error messages */

  empty_line();

/* errors for bad variable passed */

  status = pthread_cond_signal( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_signal - EINVAL (cond invalid)" );

  status = pthread_cond_broadcast( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_broadcast - EINVAL (cond invalid)" );

/* acquire mutex so errors will occur */

  status = pthread_mutex_lock( &Mutex_id );
  rtems_test_assert( !status );

  status = pthread_cond_wait( NULL, &Mutex_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_wait - EINVAL (cond invalid)" );

  status = pthread_cond_timedwait( NULL, &Mutex_id, &timeout );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_timedwait - EINVAL (cond invalid)" );

  status = pthread_cond_wait( &Cond1_id, NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_wait - EINVAL (mutex invalid)" );

  status = pthread_cond_timedwait( &Cond1_id, NULL, &timeout );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_timedwait - EINVAL (mutex invalid)" );

  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_timedwait - EINVAL (abstime NULL)" );

  status = clock_gettime( CLOCK_REALTIME, &timeout );
  rtems_test_assert( !status );
  timeout.tv_sec -= 1;
  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_timedwait - ETIMEDOUT (abstime->tv_sec < current time)" );
  status = pthread_mutex_unlock( &Mutex_id );
  rtems_test_assert( !status );

  status = pthread_mutex_lock( &Mutex_id );
  rtems_test_assert( !status );

  /* ensure we do not catch a 0 nanosecond boundary */
  do {
    status = clock_gettime( CLOCK_REALTIME, &timeout );
    rtems_test_assert( !status );
    timeout.tv_nsec -= 1;
  } while ( timeout.tv_nsec < 0);

  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_timedwait - ETIMEDOUT (abstime->tv_nsec < current time)" );
  status = pthread_mutex_unlock( &Mutex_id );
  rtems_test_assert( !status );

/* wait and timedwait without mutex */

/* XXX - this case is commented out in the code pending review
 *
 *   status = pthread_cond_wait( &Cond1_id, &Mutex_id );
 *   if ( status != EINVAL )
 *     printf( "status = %d\n", status );
 *   rtems_test_assert( status == EINVAL );
 */
  puts( "Init: pthread_cond_wait - EINVAL (mutex not locked before call)" );

/* XXX - this case is commented out in the code pending review
 *
 *  status = clock_gettime( CLOCK_REALTIME, &timeout );
 *  rtems_test_assert( !status );
 *  timeout.tv_sec += 1;
 *  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, &timeout );
 *  if ( status != EINVAL )
 *    printf( "status = %d\n", status );
 *  rtems_test_assert( status == EINVAL );
 */
  puts( "Init: pthread_cond_timedwait - EINVAL (mutex not locked before call)");

  empty_line();

  status = pthread_create( &Task3_id, NULL, Task_3, NULL );
  rtems_test_assert( !status );

/* switch to task3 to allow it to wait for broadcast signal */

  puts( "Init: sleep - switch to Task_3" );
  sleep( 1 );

/* destroy the mutex so Task3 can not acguire at the end of Wait_support */

  status = pthread_mutex_destroy( &Mutex_id );
  rtems_test_assert( !status );

/* signal a condition variable to task3 */

  puts( "Init: pthread_cond_signal" );
  status = pthread_cond_signal( &Cond1_id );

  puts( "Init: sleep - switch to Task_3" );
  sleep( 1 );

  puts( "*** END OF POSIX TEST 10 ***" );
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
