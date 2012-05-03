/*
 *  This test exercises the POSIX Barrier manager.
 *
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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/* #define __USE_XOPEN2K XXX already defined on GNU/Linux */
#include <pthread.h>

#include "tmacros.h"

#define NUMBER_THREADS 2
pthread_t ThreadIds[NUMBER_THREADS];
pthread_barrier_t Barrier;
void *BarrierThread(void *arg);

void *BarrierThread(void *arg)
{
  pthread_t id = *(pthread_t *) arg;
  int       status;

  printf( "pthread_barrier_wait( &Barrier ) for thread 0x%08" PRIxpthread_t "\n", id );
  status = pthread_barrier_wait( &Barrier );
  printf( "pthread_barrier_wait - 0x%08" PRIxpthread_t " released\n", id );
  rtems_test_assert( (status == 0) || (status == PTHREAD_BARRIER_SERIAL_THREAD) );

  return NULL;
}

/*
 *  main entry point to the test
 */

#if defined(__rtems__)
int test_main(void);

int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  pthread_barrier_t     bad_barrier = 100;
  pthread_barrier_t     barrier;
  pthread_barrierattr_t attr;
  int                   status;
  int                   p;
  int                   i;

  puts( "\n\n*** POSIX BARRIER TEST 01 ***" );

  /*************** NULL POINTER CHECKS *****************/
  puts( "pthread_barrierattr_init( NULL ) -- EINVAL" );
  status = pthread_barrierattr_init( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_setpshared( NULL, private ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( NULL, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_setpshared( NULL, shared ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( NULL, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_getpshared( NULL, &p ) -- EINVAL" );
  status = pthread_barrierattr_getpshared( NULL, &p );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_destroy( NULL ) -- EINVAL" );
  status = pthread_barrierattr_destroy( NULL );
  rtems_test_assert( status == EINVAL );

  /*************** NOT INITIALIZED CHECKS *****************/
  /* cheat visibility */
  attr.is_initialized = 0;
  puts( "pthread_barrierattr_setpshared( &attr, shared ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_getpshared( &attr, NULL ) -- EINVAL" );
  status = pthread_barrierattr_getpshared( &attr, NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrierattr_destroy( &attr ) -- EINVAL" );
  status = pthread_barrierattr_destroy( &attr );
  rtems_test_assert( status == EINVAL );


  /*************** ACTUALLY WORK THIS TIME *****************/

  puts( "pthread_barrierattr_init( &attr ) -- OK" );
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrierattr_setpshared( &attr, private ) -- OK" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  rtems_test_assert( status == 0 );
  rtems_test_assert( p == PTHREAD_PROCESS_PRIVATE );

  puts( "pthread_barrierattr_setpshared( &attr, shared ) -- OK" );
  status = pthread_barrierattr_setpshared( &attr, PTHREAD_PROCESS_SHARED );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) -- OK" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  rtems_test_assert( status == 0 );
  rtems_test_assert( p == PTHREAD_PROCESS_SHARED );

  /*************** BAD PSHARED CHECK *****************/
  puts( "pthread_barrierattr_setpshared( &attr, private ) -- EINVAL" );
  status = pthread_barrierattr_setpshared( &attr, ~PTHREAD_PROCESS_PRIVATE );
  rtems_test_assert( status == EINVAL );

  /*************** DESTROY/REUSE CHECK *****************/
  puts( "pthread_barrierattr_destroy( &attr ) -- OK" );
  status = pthread_barrierattr_destroy( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrierattr_getpshared( &attr, &p ) destroyed -- EINVAL" );
  status = pthread_barrierattr_getpshared( &attr, &p );
  rtems_test_assert( status == EINVAL );

  /*************** pthread_barrier_init ERROR CHECKs *********/
  /* NULL barrier argument */
  puts( "pthread_barrier_init( NULL, NULL, 2 ) -- EINVAL" );
  status = pthread_barrier_init( NULL, NULL, 2 );
  rtems_test_assert( status == EINVAL );

  /* uninitialized attr argument */
  puts( "pthread_barrier_init( &barrier, &attr, 2 ) -- EINVAL" );
  status = pthread_barrier_init( &barrier, &attr, 2 );
  rtems_test_assert( status == EINVAL );

  /* zero count argument */
  puts( "pthread_barrierattr_init( &attr ) -- OK" );
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrier_init( &barrier, &attr, 0 ) -- EINVAL" );
  status = pthread_barrier_init( &barrier, &attr, 0 );
  rtems_test_assert( status == EINVAL );

  /* allocating too many */
  puts( "pthread_barrier_init( &barrier, NULL, 1 ) -- OK" );
  status = pthread_barrier_init( &barrier, NULL, 1 );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrier_init( &barrier, NULL, 1 ) -- EAGAIN" );
  status = pthread_barrier_init( &barrier, NULL, 1 );
  rtems_test_assert( status == EAGAIN );

  /* clean up */
  puts( "pthread_barrier_destroy( &barrier ) -- OK" );
  status = pthread_barrier_destroy( &barrier );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrierattr_destroy( &attr ) -- OK" );
  status = pthread_barrierattr_destroy( &attr );
  rtems_test_assert( status == 0 );

  /*************** pthread_barrier_destroy ERROR CHECKs *********/
  /* NULL barrier argument */
  puts( "pthread_barrier_destroy( NULL ) -- EINVAL" );
  status = pthread_barrier_destroy( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrier_destroy( &bad_barrier ) -- EINVAL" );
  status = pthread_barrier_destroy( &bad_barrier );
  rtems_test_assert( status == EINVAL );

  /*************** pthread_barrier_wait ERROR CHECKs *********/
  /* NULL barrier argument */
  puts( "pthread_barrier_wait( NULL ) -- EINVAL" );
  status = pthread_barrier_wait( NULL );
  rtems_test_assert( status == EINVAL );

  puts( "pthread_barrier_wait( &bad_barrier ) -- EINVAL" );
  status = pthread_barrier_wait( &bad_barrier );
  rtems_test_assert( status == EINVAL );

  /*************** ACTUALLY CREATE ONE CHECK *****************/
  puts( "pthread_barrierattr_init( &attr ) -- OK" );
  status = pthread_barrierattr_init( &attr );
  rtems_test_assert( status == 0 );

  puts( "pthread_barrier_init( &barrier, &attr, 2 ) -- OK" );
  status = pthread_barrier_init( &barrier, &attr, 2 );
  rtems_test_assert( status == 0 );
  rtems_test_assert( barrier != 0 );

  puts( "pthread_barrier_destroy( &barrier ) -- OK" );
  status = pthread_barrier_destroy( &barrier );
  rtems_test_assert( status == 0 );

  /*************** CREATE THREADS AND LET THEM RELEASE *****************/
  puts( "pthread_barrier_init( &Barrier, &attr, NUMBER_THREADS ) -- OK" );
  status = pthread_barrier_init( &Barrier, &attr, NUMBER_THREADS );
  rtems_test_assert( status == 0 );
  rtems_test_assert( barrier != 0 );

  for (i=0 ; i<NUMBER_THREADS ; i++ ) {

    /* check for unable to destroy while threads waiting */
    if (i == NUMBER_THREADS - 1) {
      puts( "pthread_barrier_destroy( &Barrier ) -- EBUSY" );
      status = pthread_barrier_destroy( &Barrier );
      rtems_test_assert( status == EBUSY );
    }

    /* create a thread to block on the barrier */
    printf( "Init: pthread_create - thread %d OK\n", i+1 );
    status = pthread_create(&ThreadIds[i], NULL, BarrierThread, &ThreadIds[i]);
    rtems_test_assert( !status );

    sleep(1);
  }

  /*************** END OF TEST *****************/
  puts( "*** END OF POSIX BARRIER TEST 01 ***" );
  exit(0);
}
