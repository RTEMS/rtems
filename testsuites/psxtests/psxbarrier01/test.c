/*
 *  This test exercises the POSIX Barrier manager.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/* #define __USE_XOPEN2K XXX already defined on GNU/Linux */
#include <pthread.h>

#define NUMBER_THREADS 2
pthread_t ThreadIds[NUMBER_THREADS];
pthread_barrier_t Barrier;

void *BarrierThread(void *arg)
{
  pthread_t id = *(pthread_t *) arg;
  int       status;

  printf( "pthread_barrier_wait( &Barrier ) for thread 0x%08x\n", id );
  status = pthread_barrier_wait( &Barrier );
  if ( id == ThreadIds[NUMBER_THREADS - 1] ) {
    printf( "pthread_barrier_wait - 0x%08x auto released\n", id );
    assert( status == PTHREAD_BARRIER_SERIAL_THREAD );
  } else {
    printf( "pthread_barrier_wait - 0x%08x released\n", id );
    assert( status == 0 );
  }

  return NULL;
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  pthread_barrier_t     barrier;
  pthread_barrierattr_t attr;
  int                   status;
  int                   p;
  pthread_t             thread_id;
  int                   i;

  puts( "\n\n*** POSIX BARRIER TEST 01 ***" );

  /*************** NULL POINTER CHECKS *****************/
  puts( "pthread_barrierattr_init( NULL ) -- EINVAL" );
  status = pthread_barrierattr_init( NULL );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_setpshared( NULL, private ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_setpshared( NULL, shared ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( NULL, PTHREAD_PROCESS_SHARED );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_getpshared( NULL, &p ) -- EINVAL" );
  status = pthread_barrierattr_getpshared( NULL, &p );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_destroy( NULL ) -- EINVAL" );
  status = pthread_barrierattr_destroy( NULL );
  assert( status == EINVAL );

  /*************** NOT INITIALIZED CHECKS *****************/
  /* cheat visibility */
  attr.is_initialized = 0; 
  puts( "pthread_barrierattr_setpshared( &attr, shared ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_getpshared( &attr, NULL ) -- EINVAL" );
  status = pthread_barrierattr_getpshared( &attr, NULL );
  assert( status == EINVAL );

  puts( "pthread_barrierattr_destroy( &attr ) -- EINVAL" );
  status = pthread_barrierattr_destroy( &attr );
  assert( status == EINVAL );


  /*************** ACTUALLY WORK THIS TIME *****************/

  puts( "pthread_barrierattr_init( &attr ) -- OK" );
  status = pthread_barrierattr_init( &attr );
  assert( status == 0 );

  puts( "pthread_barrierattr_setpshared( &attr, private ) -- OK" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  assert( status == 0 );
  assert( p == PTHREAD_PROCESS_PRIVATE );

  puts( "pthread_barrierattr_setpshared( &attr, shared ) -- OK" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  assert( status == 0 );
  assert( p == PTHREAD_PROCESS_SHARED );

  /*************** BAD PSHARED CHECK *****************/
  puts( "pthread_barrierattr_setpshared( &attr, private ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( &attr, ~PTHREAD_PROCESS_PRIVATE );
  assert( status == EINVAL );

  /*************** DESTROY/REUSE CHECK *****************/
  puts( "pthread_barrierattr_destroy( &attr ) -- OK" );
  status = pthread_barrierattr_destroy( &attr );
  assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) destroyed -- EINVAL" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  assert( status == EINVAL );


  /* XXX _init error checks */

  /*************** ACTUALLY CREATE ONE CHECK *****************/
  puts( "pthread_barrierattr_init( &attr ) -- OK" );
  status = pthread_barrierattr_init( &attr );
  assert( status == 0 );

  puts( "pthread_barrier_init( &barrier, &attr, 2 ) -- OK" );
  status = pthread_barrier_init( &barrier, &attr, 2 );
  assert( status == 0 );
  assert( barrier != 0 );

  puts( "pthread_barrier_destroy( &barrier ) -- OK" );
  status = pthread_barrier_destroy( &barrier );
  assert( status == 0 );

  /*************** CREATE TESTS AND LET THEM RELEASE *****************/
  puts( "pthread_barrier_init( &Barrier, &attr, 2 ) -- OK" );
  status = pthread_barrier_init( &Barrier, &attr, 2 );
  assert( status == 0 );
  assert( barrier != 0 );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status = pthread_create(&ThreadIds[i], NULL, BarrierThread, &ThreadIds[i]);
    assert( !status );

    sleep(1);
  }

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX BARRIER TEST 01 ***" );
  exit(0);
}
