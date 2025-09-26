/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>

const char rtems_test_name[] = "PSX 10";

static void test_cond_null( void )
{
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  int eno;
  struct timespec to;
  clockid_t clock_id;

  eno = pthread_cond_init( NULL, NULL );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_lock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_wait( NULL, &mtx );
  rtems_test_assert( eno == EINVAL );

  to.tv_sec = 1;
  to.tv_nsec = 1;
  eno = pthread_cond_timedwait( NULL, &mtx, &to );
  rtems_test_assert( eno == EINVAL );

  clock_id = CLOCK_MONOTONIC;
  eno = pthread_cond_clockwait( NULL, &mtx, clock_id, &to );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_unlock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_signal( NULL );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_broadcast( NULL );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_destroy( NULL );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == 0 );
}

static void test_cond_not_initialized( void )
{
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t cond;
  int eno;
  struct timespec to;
  clockid_t clock_id;

  memset( &cond, 0xff, sizeof( cond ) );

  eno = pthread_mutex_lock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_wait( &cond, &mtx );
  rtems_test_assert( eno == EINVAL );

  to.tv_sec = 1;
  to.tv_nsec = 1;
  eno = pthread_cond_timedwait( &cond, &mtx, &to );
  rtems_test_assert( eno == EINVAL );

  clock_id = CLOCK_MONOTONIC;
  eno = pthread_cond_clockwait( &cond, &mtx, clock_id, &to );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_unlock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_signal( &cond );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_broadcast( &cond );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_destroy( &cond );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == 0 );
}

static void test_cond_invalid_copy( void )
{
  pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t cond;
  pthread_cond_t cond2;
  int eno;
  struct timespec to;
  clockid_t clock_id;

  eno = pthread_cond_init( &cond, NULL );
  rtems_test_assert( eno == 0 );

  memcpy( &cond2, &cond, sizeof( cond2 ) );

  eno = pthread_mutex_lock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_wait( &cond2, &mtx );
  rtems_test_assert( eno == EINVAL );

  to.tv_sec = 1;
  to.tv_nsec = 1;
  eno = pthread_cond_timedwait( &cond2, &mtx, &to );
  rtems_test_assert( eno == EINVAL );

  clock_id = CLOCK_MONOTONIC;
  eno = pthread_cond_clockwait( &cond2, &mtx, clock_id, &to );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_mutex_unlock( &mtx );
  rtems_test_assert( eno == 0 );

  eno = pthread_cond_signal( &cond2 );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_broadcast( &cond2 );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_destroy( &cond2 );
  rtems_test_assert( eno == EINVAL );

  eno = pthread_cond_destroy( &cond );
  rtems_test_assert( eno == 0 );

  eno = pthread_mutex_destroy( &mtx );
  rtems_test_assert( eno == 0 );
}

void *POSIX_Init(
  void *argument
)
{
  (void) argument;

  int                 status;
  pthread_condattr_t  attr;
  pthread_condattr_t  attr_error;
  int                 pshared;
  pthread_cond_t      cond;
  struct timespec     timeout;
  clockid_t clock_id;

  TEST_BEGIN();

  test_cond_null();
  test_cond_not_initialized();
  test_cond_invalid_copy();

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

/* error for bad condition variable passed */

  status = pthread_cond_destroy( NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_destroy - EINVAL (cond invalid)" );

/* pshared tests */

  puts( "Init: pthread_cond_init - EINVAL (invalid pshared)" );
  attr.process_shared = -1;
  status = pthread_cond_init( &cond, &attr );
  rtems_test_assert( status == EINVAL );

  puts( "Init: pthread_condattr_setpshared - PTHREAD_PROCESS_SHARED" );
  status = pthread_condattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == 0 );

  puts( "Init: pthread_cond_init - OK" );
  status = pthread_cond_init( &cond, &attr );
  rtems_test_assert( status == 0 );

  puts( "Init: pthread_cond_destroy - OK" );
  status = pthread_cond_destroy( &cond );
  rtems_test_assert( status == 0 );

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

/* clockwait case - timeout */

  status = pthread_mutex_lock( &Mutex_id );
  rtems_test_assert( !status );

/* set timeout to 3 seconds */

  status = clock_gettime( CLOCK_REALTIME, &timeout );
  rtems_test_assert( !status );
  timeout.tv_sec += 3;
  timeout.tv_nsec = 0;
  clock_id = CLOCK_REALTIME;

  puts( "Init: pthread_cond_clockwait for 3 seconds" );
  status = pthread_cond_clockwait( &Cond1_id, &Mutex_id, clock_id ,&timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_clockwait - ETIMEDOUT - (mutex not acquired)" );

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

  status = pthread_cond_clockwait( NULL, &Mutex_id, clock_id, &timeout );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_clockwait - EINVAL (cond invalid)" );

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

  status = pthread_cond_clockwait( &Cond1_id, NULL, clock_id, &timeout );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_clockwait - EINVAL (mutex invalid)" );

  status = pthread_cond_timedwait( &Cond1_id, &Mutex_id, NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_timedwait - EINVAL (abstime NULL)" );

  status = pthread_cond_clockwait( &Cond1_id, &Mutex_id, clock_id,NULL );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_clockwait - EINVAL (abstime NULL)" );

  clock_id = (clockid_t) 99 ; //invalid clock value
  status = pthread_cond_clockwait( &Cond1_id, NULL, clock_id, &timeout );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  rtems_test_assert( status == EINVAL );
  puts( "Init: pthread_cond_clockwait - EINVAL (clock invalid)" );

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

  status = clock_gettime( CLOCK_MONOTONIC, &timeout );
  rtems_test_assert( !status );
  timeout.tv_sec -= 1;
  clock_id = CLOCK_MONOTONIC;
  status = pthread_cond_clockwait( &Cond1_id, &Mutex_id, clock_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_clockwait - ETIMEDOUT (abstime->tv_sec < current time)" );
  status = pthread_mutex_unlock( &Mutex_id );
  rtems_test_assert( !status );

  do {
    status = clock_gettime( CLOCK_MONOTONIC, &timeout );
    rtems_test_assert( !status );
    timeout.tv_nsec -= 1;
  } while ( timeout.tv_nsec < 0);

  clock_id = CLOCK_MONOTONIC;
  status = pthread_cond_clockwait( &Cond1_id, &Mutex_id, clock_id, &timeout );
  if ( status != ETIMEDOUT )
    printf( "status = %d\n", status );
  rtems_test_assert( status == ETIMEDOUT );
  puts( "Init: pthread_cond_clockwait - ETIMEDOUT (abstime->tv_nsec < current time)" );
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

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
